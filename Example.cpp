#include "clang/Driver/Options.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Lex/Lexer.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/AST/DeclBase.h"
#include "clang/Tooling/Refactoring.h"
#include "clang/Lex/HeaderSearch.h"
#include "clang/Lex/HeaderSearchOptions.h"
#include "clang/Lex/DirectoryLookup.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/Module.h"
#include "clang/Lex/PPCallbacks.h"
#include "clang/Lex/MacroInfo.h"
#include "clang/Basic/LLVM.h"


#include "clang/Lex/Preprocessor.h"
#include "clang/Lex/PreprocessorOptions.h"
#include "clang/Tooling/CompilationDatabase.h"
#include "llvm/Support/Path.h"
#include "clang/Parse/Parser.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <stdio.h>


#include "Rplsmt.h"


using namespace std;
using namespace clang;
using namespace clang::driver;
using namespace clang::tooling;
using namespace llvm;

Rewriter rewriter;
int numFunctions = 0;
std::string fileName;
std::string dirPath;
bool rewrite_enable = false;
std::string replacementsPath = "./replacements.txt";
std::vector<std::string> compilerPredefines;
std::vector<std::string> compilerSearchPaths;
std::vector<std::string> clangSearchPaths;

#define RPLSMTS_CONTAINER_TYPE std::unordered_map<std::string, std::vector<Rplsmt>>
RPLSMTS_CONTAINER_TYPE replacements;

#define DEBUG_PRINT_EXAMPLE 0

#if defined(DEBUG_PRINT_EXAMPLE) && DEBUG_PRINT_EXAMPLE>0
	#define DEBUG_PRINT(...) fprintf (stderr,__VA_ARGS__)
#else
	#define DEBUG_PRINT(...)
#endif

int find_string (std::string &str, std::vector<std::string> &vector)
{
    for (unsigned int i = 0; i < vector.size(); i++)
    {
        if (vector[i] == str)
            return 1;
    }
    return 0;
}


// bool isCallToStdMove() const {
//      const FunctionDecl* FD = getDirectCallee();
//      return getNumArgs() == 1 && FD && FD->isInStdNamespace() &&
//             FD->getIdentifier() && FD->getIdentifier()->isStr("move");
//   }

SourceLocation findSemiAfterLocation(SourceLocation loc,
                                            ASTContext &Ctx,
                                            bool IsDecl) {
  SourceManager &SM = Ctx.getSourceManager();
  if (loc.isMacroID()) {
  //std::cout << __func__ << " " << __LINE__ << std::endl;
    if (!Lexer::isAtEndOfMacroExpansion(loc, SM, Ctx.getLangOpts(), &loc))
    {
      //std::cout << __func__ << " " << __LINE__ << std::endl;
      return SourceLocation();
    }
  }
  loc = Lexer::getLocForEndOfToken(loc, /*Offset=*/0, SM, Ctx.getLangOpts());

  // Break down the source location.
  std::pair<FileID, unsigned> locInfo = SM.getDecomposedLoc(loc);

  // Try to load the file buffer.
  bool invalidTemp = false;
  StringRef file = SM.getBufferData(locInfo.first, &invalidTemp);
  if (invalidTemp)
    return SourceLocation();

  const char *tokenBegin = file.data() + locInfo.second;

  // Lex from the start of the given location.
  Lexer lexer(SM.getLocForStartOfFile(locInfo.first),
              Ctx.getLangOpts(),
              file.begin(), tokenBegin, file.end());
  Token tok;
  lexer.LexFromRawLexer(tok);
  if (tok.isNot(tok::semi)) {
    //std::cout << __func__ << " " << __LINE__ << std::endl;
  if (!IsDecl)
  {
    //if (std::string(tok.getName())=="rawIdentifier")
    return tok.getLocation().getLocWithOffset(tok.getRawIdentifier().str().size()); //return raw identifier end
    //return SourceLocation();
  }
      
    // Declaration may be followed with other tokens; such as an __attribute,
    // before ending with a semicolon.
    //std::cout << __func__ << " " << __LINE__ << std::endl;
    return findSemiAfterLocation(tok.getLocation(), Ctx, /*IsDecl*/true);
  }

  return tok.getLocation();
}

// Make the Path absolute using the current working directory of the given
// SourceManager if the Path is not an absolute path.
//
// The Path can be a path relative to the build directory, or retrieved from
// the SourceManager.
std::string MakeAbsolutePath(const clang::SourceManager &SM, clang::StringRef Path) {
  llvm::SmallString<128> AbsolutePath(Path);
  if (std::error_code EC = SM.getFileManager().getVirtualFileSystem()->makeAbsolute(AbsolutePath))
    llvm::errs() << "Warning: could not make absolute file: '" << EC.message() << '\n';
  // Handle symbolic link path cases.
  // We are trying to get the real file path of the symlink.
  const clang::DirectoryEntry *Dir = SM.getFileManager().getDirectory(llvm::sys::path::parent_path(AbsolutePath.str()));
  if (Dir) {
    clang::StringRef DirName = SM.getFileManager().getCanonicalName(Dir);
    llvm::SmallVector<char, 128> AbsoluteFilename;
    llvm::sys::path::append(AbsoluteFilename, DirName, llvm::sys::path::filename(AbsolutePath.str()));
    return llvm::StringRef(AbsoluteFilename.data(), AbsoluteFilename.size()).str();
  }
  return AbsolutePath.str();
}

class Find_Includes : public PPCallbacks
{
public:
  bool has_include;

  void InclusionDirective(
    clang::SourceLocation hash_loc,
    const clang::Token &include_token,
    clang::StringRef file_name,
    bool is_angled,
    clang::CharSourceRange filename_range,
    const clang::FileEntry *file,
    clang::StringRef search_path,
    clang::StringRef relative_path,
    const clang::Module *imported)
  {
    // do something with the include
    has_include = true;
  }
};

class Find_Macros : public PPCallbacks
{
public:
  bool has_macro;



  void Ifdef  ( clang::SourceLocation Loc,
    const clang::Token &MacroNameTok,
    const clang::MacroDefinition &MD 
    ) 
  {
    // do something with the include
    has_macro = true;

    //clang::SourceManager &sm = compiler.getSourceManager();

        std::string filePath = MakeAbsolutePath(rewriter.getSourceMgr(),rewriter.getSourceMgr().getFilename(rewriter.getSourceMgr().getSpellingLoc(MacroNameTok.getLocation())));
        std::string MacroName = MacroNameTok.getIdentifierInfo()->getName();
        int MacroLength = MacroNameTok.getLength();
        
        std::cout << "ifdef " << "\"" << MacroName << "\"" << " " << MacroLength << " " << filePath << "\n";

    // if (sm.isInMainFile(hashLoc)) {
    //     const unsigned int lineNum = sm.getSpellingLineNumber(hashLoc);
    //     includes.push_back(std::make_pair(lineNum, fileName));
    // }

  }

  void Ifndef  ( clang::SourceLocation Loc,
  const clang::Token &MacroNameTok,
  const clang::MacroDefinition &MD 
  ) 
  {
    // do something with the include
    has_macro = true;

    //clang::SourceManager &sm = compiler.getSourceManager();

        std::string filePath = MakeAbsolutePath(rewriter.getSourceMgr(),rewriter.getSourceMgr().getFilename(rewriter.getSourceMgr().getSpellingLoc(MacroNameTok.getLocation())));
        std::string MacroName = MacroNameTok.getIdentifierInfo()->getName();
        int MacroLength = MacroNameTok.getLength();
        
        std::cout << "ifndef " << "\"" << MacroName << "\"" << " " << MacroLength << " " << filePath << "\n";

    // if (sm.isInMainFile(hashLoc)) {
    //     const unsigned int lineNum = sm.getSpellingLineNumber(hashLoc);
    //     includes.push_back(std::make_pair(lineNum, fileName));
    // }

  }


  void Defined  ( const clang::Token &MacroNameTok,
                  const clang::MacroDefinition &MD,
                  clang::SourceRange Range) 
  {
    //clang::SourceManager &sm = compiler.getSourceManager();

        std::string filePath = MakeAbsolutePath(rewriter.getSourceMgr(),rewriter.getSourceMgr().getFilename(rewriter.getSourceMgr().getSpellingLoc(MacroNameTok.getLocation())));
        std::string MacroName = MacroNameTok.getIdentifierInfo()->getName();
        int MacroLength = MacroNameTok.getLength();
        
        std::cout << "defined " << "\"" << MacroName << "\"" << " " << MacroLength << " " << filePath << "\n";

        
    // if (sm.isInMainFile(hashLoc)) {
    //     const unsigned int lineNum = sm.getSpellingLineNumber(hashLoc);
    //     includes.push_back(std::make_pair(lineNum, fileName));
    // }

  }



void If ( clang::SourceLocation Loc,
          clang::SourceRange ConditionRange,
           clang::PPCallbacks::ConditionValueKind ConditionValue
      ) 
  {
    //clang::SourceManager &sm = compiler.getSourceManager();

        std::string filePath = MakeAbsolutePath(rewriter.getSourceMgr(),rewriter.getSourceMgr().getFilename(rewriter.getSourceMgr().getSpellingLoc(Loc)));
		CharSourceRange charSourceRange = clang::CharSourceRange::getCharRange(ConditionRange);
		std::string patternLine = Lexer::getSourceText(charSourceRange,rewriter.getSourceMgr(),rewriter.getLangOpts());
        std::cout << "If: " << "\"" << patternLine << "\"" << " " << filePath << "\n";
  }


void Elif ( clang::SourceLocation Loc,
          clang::SourceRange ConditionRange,
           clang::PPCallbacks::ConditionValueKind ConditionValue,
           clang::SourceLocation IfLoc
      ) 
  {
    //clang::SourceManager &sm = compiler.getSourceManager();

        std::string filePath = MakeAbsolutePath(rewriter.getSourceMgr(),rewriter.getSourceMgr().getFilename(rewriter.getSourceMgr().getSpellingLoc(Loc)));
		CharSourceRange charSourceRange = clang::CharSourceRange::getCharRange(ConditionRange);
		std::string patternLine = Lexer::getSourceText(charSourceRange,rewriter.getSourceMgr(),rewriter.getLangOpts());
        std::cout << "Elif: " << "\"" << patternLine << "\"" << " " << filePath << "\n";
  }

  void MacroExpands ( const clang::Token &MacroNameTok,
                    const clang::MacroDefinition &MD,
                    clang::SourceRange Range,
                    const clang::MacroArgs *Args) 
  {
    //clang::SourceManager &sm = compiler.getSourceManager();

        std::string filePath = MakeAbsolutePath(rewriter.getSourceMgr(),rewriter.getSourceMgr().getFilename(rewriter.getSourceMgr().getSpellingLoc(MacroNameTok.getLocation())));
        std::string MacroName = MacroNameTok.getIdentifierInfo()->getName();
        int MacroLength = MacroNameTok.getLength();
        
        std::cout << "MacroExpands " << "\"" << MacroName << "\"" << " " << MacroLength << " " << filePath << "\n";

    // if (sm.isInMainFile(hashLoc)) {
    //     const unsigned int lineNum = sm.getSpellingLineNumber(hashLoc);
    //     includes.push_back(std::make_pair(lineNum, fileName));
    // }

  }


void MacroDefined 	(const clang::Token &MacroNameTok,
					const clang::MacroDirective *MD)
{

	std::string filePath = MakeAbsolutePath(rewriter.getSourceMgr(),rewriter.getSourceMgr().getFilename(rewriter.getSourceMgr().getSpellingLoc(MacroNameTok.getLocation())));
	std::string MacroName = MacroNameTok.getIdentifierInfo()->getName();
	int MacroLength = MacroNameTok.getLength();

    std::cout << "MacroDefined " << "\"" << MacroName << "\"" << " " << MacroLength << " " << filePath << "\n";

}




};

// Find_Macros::Find_Macros(const clang::CompilerInstance &compiler)
// {
//   this->compiler = compiler;
// }

int getPredefines (const std::string &compilerName, std::vector<std::string> &predefines)
{

  std::string command = "";
  std::fstream inFile;
  std::string line;
  std::string inFilePath = "./predefines.txt";

  if (compilerName == "gcc")
  {
    command = "gcc -dM -E -x c /dev/null > " + inFilePath;
    system(command.c_str());
  }
  else if (compilerName == "g++")
  {
    command = "g++ -x c++ -dM -E - < /dev/null > " + inFilePath;
    system(command.c_str());
  }
  else if (compilerName == "clang")
  {
    command = "clang -dM -E -x c /dev/null > " + inFilePath;
    system(command.c_str());
  }

  inFile.open(inFilePath, std::ios::in);

  if (inFile.is_open())
  {
    while (std::getline(inFile,line))
    {

        predefines.push_back(line);
    }

    inFile.close();
  }
  else
  {
    std::cout << "Error opening input file: " << inFilePath << "\n\r";
    inFile.close();
    return 0;
  }

inFile.close();
// outFile.open(outFilePath, std::ios::out | std::ios::in | std::ios::trunc);
// outFile << line << " " << level << std::endl;
// outFile.close();

return 1;
}


int getSearchPaths (const std::string &compilerName, std::vector<std::string> &predefines)
{

  std::string command = "";
  std::fstream inFile;
  std::string line;
  std::string inFilePath = "./compilerSearchPaths.txt";

  if (compilerName == "gcc")
  {
    command = "echo | gcc -E -Wp,-v - /dev/null > " + inFilePath + " 2>&1";
    system(command.c_str());
  }
  else if (compilerName == "g++")
  {
    command = "g++ -E -x c++ - -v < /dev/null > " + inFilePath + " 2>&1";
    system(command.c_str());
  }
  else if (compilerName == "clang")
  {
    command = "echo | clang -E -Wp,-v - /dev/null > " + inFilePath + " 2>&1";
    system(command.c_str());
  }

  inFile.open(inFilePath, std::ios::in);
  int readIncludes = 0;

  if (inFile.is_open())
  {
    while (std::getline(inFile,line))
    {
		
		if (line.find("#include <...> search starts here") != std::string::npos)
		{
			readIncludes = 1;
		}
		else if (line.find("End of search list") != std::string::npos)
		{
			readIncludes = 0;
		}
		else if (readIncludes == 1)
		{
			predefines.push_back(line.substr(line.find("/")));
		}
    }

    inFile.close();
  }
  else
  {
    std::cout << "Error opening input file: " << inFilePath << "\n\r";
    inFile.close();
    return 0;
  }

inFile.close();
// outFile.open(outFilePath, std::ios::out | std::ios::in | std::ios::trunc);
// outFile << line << " " << level << std::endl;
// outFile.close();

return 1;
}





class ExampleVisitor : public RecursiveASTVisitor<ExampleVisitor> {
private:
    ASTContext *astContext; // used for getting additional AST info

public:
    explicit ExampleVisitor(CompilerInstance *CI) 
      : astContext(&(CI->getASTContext())) // initialize private members
    {
        //LangOptions langOpts = astContext->getLangOpts();
        //langOpts.CPlusPlus = 1;  
        //rewriter.setSourceMgr(astContext->getSourceManager(), langOpts);
        rewriter.setSourceMgr(astContext->getSourceManager(), astContext->getLangOpts());
    }

    virtual bool VisitFunctionDecl(FunctionDecl *func) {
        numFunctions++;
        string funcName = func->getNameInfo().getName().getAsString();
        SourceLocation braceLoc; 
        std::string filePath; 
        int file_processed = 0;
//        std::string idString = "";
        std::string funcParamsString = "";
        //clang::ArrayRef< ParmVarDecl * > funcParams = func->parameters();
        //clang::QualType funcRetType = func->getReturnType();

        clang::SourceManager &sm = rewriter.getSourceMgr();

        //if (funcName == "do_math")
        {
            if (func->isThisDeclarationADefinition () && func->hasBody())
            {
                filePath = MakeAbsolutePath(rewriter.getSourceMgr(),rewriter.getSourceMgr().getFilename(sm.getSpellingLoc(func->getBody()->getLocStart())));

//                idString = filePath + " " + func->getQualifiedNameAsString() + " ";

//                for (unsigned int i = 0; i < funcParams.size(); i++)
//                {
//                	funcParamsString += funcParams[i]->getQualifiedNameAsString() + " ";
//                }
//
//                idString += funcParamsString + " ";
//                idString += funcRetType.getAsString() + " ";



                //file_processed = find_string(idString, processed_files);
                SourceLocation startPattern = sm.getSpellingLoc(func->getBody()->getLocStart());
//                SourceLocation endPattern = func->getBody()->getLocStart().getLocWithOffset(10);
                SourceLocation endPattern = sm.getSpellingLoc(func->getBody()->getLocStart().getLocWithOffset(11));
                CharSourceRange charSourceRange = clang::CharSourceRange::getCharRange(startPattern,endPattern);
                std::string patternLine = Lexer::getSourceText(charSourceRange,rewriter.getSourceMgr(),rewriter.getLangOpts());

                //std::cout << "\r\n" << patternLine << std::endl;

//                DEBUG_PRINT (std::string("pattern "  + patternLine + "\n\r").c_str());
                if (patternLine.find("DEFBEG") != std::string::npos)
                {
                	file_processed = 1;
                }

//                file_processed = 0;
                int semiPos = 0;

                //std::cout << filePath << "**\n" ;
                if ((filePath.find(dirPath) == 0) && (file_processed == 0)) //is in directory and not processed yet
                {
//                	DEBUG_PRINT (std::string("rw "  + filePath + "\n\r").c_str());
                    //braceLoc = func->getBody()->getLocStart().getLocWithOffset(1);            
                    braceLoc = func->getBody()->getLocStart();            
                    if (braceLoc.isValid())
                    {

                        /* Start prevent errors in construction like

                        #define func \
                        { \
                        }

                        where braceLoc.getLocWithOffset(1) return position at line #define func \ before '\'
                        so result would be (error):

                        #define func \ DEFBEG_777
                        { \
                        }

                       */
                        int semiOffset = 0;
                        while (1)
                        {
                          startPattern = sm.getSpellingLoc(braceLoc);
          //                SourceLocation endPattern = func->getBody()->getLocStart().getLocWithOffset(10);
                          endPattern = sm.getSpellingLoc(braceLoc.getLocWithOffset(semiOffset));

                          charSourceRange = clang::CharSourceRange::getCharRange(startPattern,endPattern);
                          patternLine = Lexer::getSourceText(charSourceRange,rewriter.getSourceMgr(),rewriter.getLangOpts());

                          // std::cout << patternLine << std::endl;

                          if ( (semiPos = patternLine.find("{")) != std::string::npos) break;
                          else
                          {
                            semiOffset++;
                          }
                          

                        }

                        // std::cout << semiPos+1 << std::endl;
                        braceLoc = func->getBody()->getLocStart().getLocWithOffset(semiPos+1);
                        /* end */

                    		//rewriter.InsertTextAfter(braceLoc, " /*DEFBEG_777*/ ");
                    		long int pos = sm.getFileOffset(sm.getSpellingLoc(braceLoc));
                    		DEBUG_PRINT (std::string(filePath + " B " + std::to_string(pos) + "\n\r").c_str());
                    		replacements[filePath].push_back(Rplsmt(pos,"B"));
                    }
                    else
                    	DEBUG_PRINT ("braceLoc is invalid=\n\r");
//                    if (sm.isInMainFile(func->getDefinition()->getBodyRBrace()))
                        //rewriter.InsertTextAfter(func->getDefinition()->getBodyRbrace(), " DEFEND_777 ");
					//rewriter.InsertTextAfter(func->getBody()->getLocEnd(), " /*DEFEND_777*/ ");
                    
                    long int pos = sm.getFileOffset(sm.getSpellingLoc(func->getBody()->getLocEnd()));
					DEBUG_PRINT (std::string(filePath + " E " + std::to_string(pos) + "\n\r").c_str());
					replacements[filePath].push_back(Rplsmt(pos,"E"));

//					rewriter.InsertTextAfter(func->getBody()->getLocEnd(), "");

//					if (rewrite_enable) rewriter.overwriteChangedFiles();
                    //myfile << filePath << "\n";
                }
            }



            //rewriter.ReplaceText(func->getLocation(), funcName.length(), "add5");
            //errs() << "** Rewrote function def: " << funcName << "\n";
        }    
        return true;
    }

    virtual bool VisitStmt(Stmt *st) {
         if (ReturnStmt *ret = dyn_cast<ReturnStmt>(st)) {
        	 clang::SourceManager &sm = rewriter.getSourceMgr();
        	 std::string filePath = MakeAbsolutePath(rewriter.getSourceMgr(),rewriter.getSourceMgr().getFilename(sm.getSpellingLoc(ret->getLocStart())));

//        	 DEBUG_PRINT (std::string(filePath + " return found at offset " + std::to_string(sm.getFileOffset(sm.getSpellingLoc(ret->getLocStart().getLocWithOffset(6)))) + "\n\r").c_str());
//        	         	 DEBUG_PRINT (std::string(filePath + " return found at offset " + std::to_string(sm.getFileOffset(sm.getSpellingLoc(Lexer::findLocationAfterToken(ret->getLocStart(),clang::tok::TokenKind::string_literal ,rewriter.getSourceMgr(), rewriter.getLangOpts(),false)))) + "\n\r").c_str());

        	 Expr* retExpr = ret->getRetValue();
        	 SourceLocation retExprEndLoc;
             bool returnVoid = false;

           	 if (retExpr != NULL)
           	 {
           		 //std::cout << (retExpr->getType()).getAsString() << std::endl;
                 if ((retExpr->getType()).getAsString() != "")
                 {
                    //std::cout << "ret not Void" << std::endl;
                    returnVoid = false;
                }
                 else
                 {
                    //std::cout << "ret void" << std::endl;
                    returnVoid = true;
                }
           	 }
           	 else
           	 {
                 //std::cout << "retExpr = NULL" << std::endl;
                 returnVoid = true;
           	 }

        	 retExprEndLoc = sm.getSpellingLoc(ret->getLocEnd());

        	 int  file_processed = 0;
        	 SourceLocation startPattern = sm.getSpellingLoc(ret->getLocStart()).getLocWithOffset(-11);
        	 //             SourceLocation startPattern = sm.getSpellingLoc(ret->getLocStart()).getLocWithOffset(-6);
//             SourceLocation startPattern = sm.getSpellingLoc(ret->getLocStart()).getLocWithOffset(-10);
			 SourceLocation endPattern = sm.getSpellingLoc(ret->getLocStart());
			 //CharSourceRange charSourceRange = clang::CharSourceRange::getCharRange(endPattern,startPattern);
			 CharSourceRange charSourceRange = clang::CharSourceRange::getCharRange(startPattern,endPattern);

			 std::string patternLine = Lexer::getSourceText(charSourceRange,rewriter.getSourceMgr(),rewriter.getLangOpts());
			 //std::cout << "\r\n" << patternLine << std::endl;
			 
			 //SourceRange sourceRange = clang::SourceRange(endPattern,startPattern);
			 //std::string patternLine = rewriter.getRewrittenText(sourceRange);

//			 DEBUG_PRINT (std::string("pattern "  + patternLine + "\n\r").c_str());

			 if (patternLine.find("DEFRET") != std::string::npos)
			 {
				file_processed = 1;
			 }
			 
			 //std::cout << sm.getFileOffset(sm.getExpansionLoc(ret->getLocEnd()).getLocWithOffset(0)) << " " << sm.getFileOffset(sm.getExpansionLoc(ret->getLocStart()).getLocWithOffset(0)) << std::endl;


        	 //SourceLocation returnSemiLoc  = sm.getSpellingLoc(Lexer::findLocationAfterToken(retExprEndLoc,clang::tok::TokenKind::semi,rewriter.getSourceMgr(), rewriter.getLangOpts(),false));
			 SourceLocation returnSemiLoc  = sm.getSpellingLoc(ret->getLocStart());
			 if (returnSemiLoc.isValid() && (filePath.find(dirPath) == 0) && (file_processed == 0))
			 {
				 long int pos = sm.getFileOffset(returnSemiLoc);
				 DEBUG_PRINT (std::string(filePath + " R " + std::to_string(pos) + "\n\r").c_str());
				 replacements[filePath].push_back(Rplsmt(pos,"R"));

			 }
			 //        	 SourceLocation returnSemiLoc  = sm.getSpellingLoc(ret->getLocStart().getLocWithOffset(6));
//
//        	 if (returnSemiLoc.isValid() && (file_processed == 0) && (filePath.find(dirPath) == 0))
//        	 {
////        		 rewriter.InsertTextAfter(sm.getSpellingLoc(ret->getLocStart()), "/*DRS7*/ ");
////        		 rewriter.InsertTextBefore(returnSemiLoc, " /*DRE7*/ ");
//                 long int pos = sm.getFileOffset(sm.getSpellingLoc(returnSemiLoc));
//
//        		 if (returnVoid == false)
//        		 {
//        			 //std::cout << sm.getFileOffset(findSemiAfterLocation(ret->getLocStart(), *(this->astContext),false)) << std::endl;
//        			 DEBUG_PRINT (std::string(filePath + " R1 " + std::to_string(pos) + "\n\r").c_str());
//        			 replacements[filePath].push_back(Rplsmt(pos,"R1"));
//        		 }
//        		 else
//        		 {
//        			 returnSemiLoc = sm.getSpellingLoc(ret->getLocStart());
//        			 pos = sm.getFileOffset(sm.getSpellingLoc(returnSemiLoc));
//
//        			 DEBUG_PRINT (std::string(filePath + " R2 " + std::to_string(pos) + "\n\r").c_str());
//        			 replacements[filePath].push_back(Rplsmt(pos,"R2"));
//
////        			 returnSemiLoc = sm.getSpellingLoc(findSemiAfterLocation(ret->getLocEnd(), *(this->astContext),false));
////        			 pos = sm.getFileOffset(sm.getSpellingLoc(returnSemiLoc).getLocWithOffset(1));
////
////        			 DEBUG_PRINT (std::string(filePath + " R3 " + std::to_string(pos) + "\n\r").c_str());
////        			 replacements[filePath].push_back(Rplsmt(pos,"R3"));
//
////        			 std::cout << sm.getFileOffset(sm.getExpansionLoc(findSemiAfterLocation(ret->getLocEnd(), *(this->astContext),false))) << std::endl;
//
//
////        			 if (ret->getLocEnd().isMacroID())
////        				 {
////        				 std::cout << "isMacroID" << "\r\n";
////        				 std::cout <<  std::cout << sm.getFileOffset(sm.getSpellingLoc(ret->getLocEnd()).getLocWithOffset(0)) << std::endl;
////        				 }
//
//        		 }
//
//        		 //rewriter.InsertTextAfter(sm.getSpellingLoc(returnSemiLoc), " /*DRS7*/ ");
////        		         		 rewriter.InsertTextBefore(returnSemiLoc, "");
////        		 if (rewrite_enable) rewriter.overwriteChangedFiles();
//        	 }

//             rewriter.InsertTextAfter(ret->getLocStart(), "{ DEFRET_777 ");
             //rewriter.ReplaceText(ret->getRetValue()->getLocStart(), 6, "val");
//             SourceLocation rbraceLoc = ret->getRetValue()->getLocEnd().getLocWithOffset(0);
//             int offset = Lexer::MeasureTokenLength(rbraceLoc,
//                                        rewriter.getSourceMgr(),
//                                        rewriter.getLangOpts()) + 1;

//             rbraceLoc = rbraceLoc.getLocWithOffset(offset);
 //.getLocWithOffset(1)
//             if (rbraceLoc.isValid())
//             {
//                 rewriter.InsertTextAfter(rbraceLoc, "} ");
//                 //std::cout << "Source location = %s" << rbraceLoc.printToString(rewriter.getSourceMgr());
//             }
//             else
//                 printf ("rbraceLoc is invalid=\n");

             //errs() << "** Rewrote ReturnStmt\n";
         }
        //if (CallExpr *call = dyn_cast<CallExpr>(st)) {
        //    rewriter.ReplaceText(call->getLocStart(), 7, "add5");
        //    errs() << "** Rewrote function call\n";
        //}
        return true;
    }

/*
    virtual bool VisitReturnStmt(ReturnStmt *ret) {
        rewriter.ReplaceText(ret->getRetValue()->getLocStart(), 6, "val");
        errs() << "** Rewrote ReturnStmt\n";
        return true;
    }*/

   virtual bool VisitCallExpr(CallExpr *call) {


    if (call != NULL){
		clang::SourceManager &sm = rewriter.getSourceMgr();
		clang::SourceLocation callStart = sm.getSpellingLoc(call->getLocStart());

		if (callStart.isValid() != true)
		{
			std::cout << "callStart is not valid" << std::endl;
			return true;
		}


		//prevent from using an empty filename path
		std::string shortFilePath = sm.getFilename(sm.getSpellingLoc(call->getLocStart())).str();
		if (shortFilePath.size() == 0)
			return true;

		//check if file in project directory to prevent corrupting system files
		std::string fullFilePath = MakeAbsolutePath(sm,sm.getFilename(sm.getSpellingLoc(call->getLocStart())));
		if (fullFilePath.find(dirPath) != 0)
			return true;


        clang::QualType q = call->getType();
        const clang::Type *t = q.getTypePtrOrNull();

        if(t != NULL)
        {
            FunctionDecl *func = call->getDirectCallee(); //gives you callee function

            if (func == NULL)
            	return true;

            long int pos = sm.getFileOffset(sm.getSpellingLoc(call->getLocStart()));

            string callee = func->getNameInfo().getName().getAsString();
            cout << func->getIdentifier()->isStr("printf") << fullFilePath << " " << callee << " is called at " << pos << std::endl;
        }
    }


       // SourceLocation callEnd = sm.getSpellingLoc(call->getLocEnd());

       // SourceLocation returnSemiLoc  = sm.getSpellingLoc(Lexer::findLocationAfterToken(callEnd,clang::tok::TokenKind::semi ,rewriter.getSourceMgr(), rewriter.getLangOpts(),false));
       // if (returnSemiLoc.isValid())
       // {
       // 	CharSourceRange charSourceRange = clang::CharSourceRange::getCharRange(callStart,returnSemiLoc);
       // 	std::string patternLine = Lexer::getSourceText(charSourceRange,rewriter.getSourceMgr(),rewriter.getLangOpts());

       // 	std::replace(patternLine.begin(),patternLine.end(), '"','\'');

       // 	std::string startPrint = "printf (\"call " + patternLine + " \\n\\r\");";
       // 	std::string endPrint = "printf (\"after " + patternLine + " \\n\\r\");";

       //  	rewriter.InsertTextAfter(callStart, startPrint);
       //     rewriter.InsertTextBefore(returnSemiLoc, endPrint);
       // }



       //errs() << "** Rewrote function call\n";
       return true;
   }

};



class ExampleASTConsumer : public ASTConsumer {
private:
    ExampleVisitor *visitor; // doesn't have to be private

public:
    // override the constructor in order to pass CI
    explicit ExampleASTConsumer(CompilerInstance *CI)
        : visitor(new ExampleVisitor(CI)) // initialize the visitor
    { }

    // override this to call our ExampleVisitor on the entire source file
    virtual void HandleTranslationUnit(ASTContext &Context) {
        /* we can use ASTContext to get the TranslationUnitDecl, which is
             a single Decl that collectively represents the entire source file */
        visitor->TraverseDecl(Context.getTranslationUnitDecl());
    }

/*
    // override this to call our ExampleVisitor on each top-level Decl
    virtual bool HandleTopLevelDecl(DeclGroupRef DG) {
        // a DeclGroupRef may have multiple Decls, so we iterate through each one
        for (DeclGroupRef::iterator i = DG.begin(), e = DG.end(); i != e; i++) {
            Decl *D = *i;    
            visitor->TraverseDecl(D); // recursively visit each AST node in Decl "D"
        }
        return true;
    }
*/
};



class ExampleFrontendAction : public ASTFrontendAction {
public:
    virtual std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI, StringRef file) {
        fileName = std::string (file);

        //Without next line clang will print errors when use gcc predefines and include search paths, for example if include <stddef.h>
        //clang tool will print error:
        ///usr/lib/gcc/i686-redhat-linux/5.3.1/include/stddef.h:328:24: error: cannot combine with previous 'int' declaration
        //    specifier
        //typedef __WCHAR_TYPE__ wchar_t;
        //                       ^
        //When clang reaches errors limit it says that it stops immediately, but I don't know whether it finishes file processing.
        //Next line allows to suppress all warnings and errors but I don't know whether it changes clang behavior.
        //Simple example with above error processed correctly despite error.
        //CI.getDiagnostics().setClient(new IgnoringDiagConsumer());

        return (std::unique_ptr<ASTConsumer>)(new ExampleASTConsumer(&CI)); // pass CI pointer to ASTConsumer
    }

    ~ExampleFrontendAction()
    {
    	//if (rewrite_enable) rewriter.overwriteChangedFiles();
    }


    virtual bool BeginSourceFileAction   (CompilerInstance &  CI)
    {
   //      clang::Preprocessor &pp = CI.getPreprocessor();
 
   //      std::unique_ptr<Find_Includes> find_includes_callback(new Find_Includes());
   //      pp.addPPCallbacks(std::move(find_includes_callback));

   //      std::unique_ptr<Find_Macros> find_macros_callback(new Find_Macros());
   //      pp.addPPCallbacks(std::move(find_macros_callback));


   //      std::string predefines = "";

   //      std::string initialPredefines = pp.getPredefines();
   //      long int pos = initialPredefines.find("# 1 \"<command line>\" 1");
   //      if (pos != std::string::npos)
   //      {
   //        initialPredefines = initialPredefines.substr(pos);
   //      }

   //      initialPredefines.append("\r\n");
   //      //std::cout << "Initial predefines : \r\n" << initialPredefines << std::endl;       

   //      predefines.append("# 1 \"<built-in>\" 1");
   //      predefines.append("\r\n");

   //      for (unsigned int i = 0; i < compilerPredefines.size(); i++)
   //      {
   //        predefines.append(compilerPredefines[i]);
   //        predefines.append("\r\n");
   //      }
   //      //predefines.append("#define DEFBEG_777 \n");
   //      //predefines.append("#define DEFEND_777 \n");
   //      //predefines.append("#define DRS7  \n");
   //      //predefines.append("#undef __SSE__ \n");

   //      predefines.append(initialPredefines);

   //      //std::cout << "predefines : \r\n" << predefines << std::endl;
   //      pp.setPredefines(predefines);
   //      //std::cout << pp.getPredefines() << " |||\n";

   //      clang::HeaderSearch &headerSearch = pp.getHeaderSearchInfo();
   //      clang::HeaderSearchOptions &headerSearchOptions = headerSearch.getHeaderSearchOpts();
   //      clang::HeaderSearchOptions newHeaderSearchOptions;
   //      std::vector< clang::HeaderSearchOptions::Entry>  userEntries = headerSearchOptions.UserEntries;

   //      int quotedPathsNum = 0;
   //      int angledPathsNum = 0;

   //      /*copy search paths that go from json compile commands*/
   //      for (int i = 0; i < userEntries.size(); i++)
   //      {

		 //  if (clangSearchPaths.size() != 0)
		 //  {
			//   if (userEntries[i].Path.find(clangSearchPaths[0]) != std::string::npos)
			//   {
			// 	break; //break at clang includes
			//   }
			//   else
			//   {
			// 	newHeaderSearchOptions.UserEntries.push_back(userEntries[i]);
			// 	quotedPathsNum++;
			//   }
		 //  }
   //      }
        
   //      /*Add compiler search paths*/
   //      angledPathsNum = 0;
   //      for (unsigned int i = 0; i < compilerSearchPaths.size(); i++)
   //      {
			// newHeaderSearchOptions.AddPath(compilerSearchPaths[i].c_str(),
			// 		clang::frontend::Angled,
			// 		false,
			// 		false);
			// angledPathsNum++;
   //      }
   
   //      headerSearchOptions.UserEntries = newHeaderSearchOptions.UserEntries;

   //      std::vector<clang::DirectoryLookup> lookups;
   //      for (auto entry : headerSearchOptions.UserEntries) {
   //        //std::cout << entry.Path << std::endl;
   //          clang::DirectoryLookup lookup = clang::DirectoryLookup(CI.getFileManager().getDirectory(entry.Path), clang::SrcMgr::CharacteristicKind::C_System, false);
   //          if (!lookup.getDir())
   //          {
   //              std::cout << std::endl << "Error: Clang could not interpret path \"" << entry.Path << "\"" << std::endl;
   //              continue; //skip not existed paths (in compile_coomands.json for mesa 17.2.4 was not existed path /mesa-17.2.4/src/gallium/drivers/softpipe/include)
   //                        //that causes segmentation fault
   //              //throw SpecificError<ClangCouldNotInterpretPath>(a, where, "Clang could not interpret path " + entry.Path);
   //          }
   //          lookups.push_back(lookup);
   //      }
   //      headerSearch.SetSearchPaths(lookups, quotedPathsNum, angledPathsNum, false);


   //      // headerSearchOptions.UseStandardSystemIncludes = 0;
   //      // headerSearchOptions.UseStandardCXXIncludes = 0;
   //      // headerSearchOptions.UseBuiltinIncludes = 0;
   //      // headerSearchOptions.UseLibcxx = 0;

   //      // std::cout << "Start headers" << std::endl;
   //      // for (int i = 0; i < headerSearchOptions.UserEntries.size(); i++)
   //      // {
   //      //   std::cout << headerSearchOptions.UserEntries[i].Path << std::endl;
   //      // }

   //      // std::vector< clang::HeaderSearchOptions::SystemHeaderPrefix>   systemHeaderPrefixes = headerSearchOptions.SystemHeaderPrefixes;
   //      // for (int i = 0; i < systemHeaderPrefixes.size(); i++)
   //      // {
   //      //   std::cout << systemHeaderPrefixes[i].Prefix << std::endl;
   //      // }

        return true;
    }

    virtual void EndSourceFileAction   ()
    {


	// CompilerInstance &ci = getCompilerInstance();
	// Preprocessor &pp = ci.getPreprocessor();
	// Find_Includes *find_includes_callback = static_cast<Find_Includes*>(pp.getPPCallbacks());
	// Find_Macros *find_macros_callback = static_cast<Find_Macros*>(pp.getPPCallbacks());
    // // do whatever you want with the callback now
    // if (find_includes_callback->has_include)
    //   std::cout << "Found at least one include" << std::endl;

    // if (find_macros_callback->has_macro)
    //   std::cout << "Found at least one macro" << std::endl;


        // clang::PreprocessorOptions &po = CI.getPreprocessorOpts();
        // po.addMacroDef(llvm::StringRef("DEFBEG_777=\"\""));
    	//SourceManager &SM = rewriter.getSourceMgr();
//		llvm::errs() << "** EndSourceFileAction for: "
//					 << SM.getFileEntryForID(SM.getMainFileID())->getName() << "\n";
//		rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(errs());
//		errs() << "\n\r";
    }


};

static cl::OptionCategory MyToolCategory("My tool options");

int main(int argc, const char **argv) {


	std::string err_str;
    std::string line;
    std::fstream myfile;
    int result = 0;

	static std::unique_ptr< CompilationDatabase > cdb = clang::tooling::CompilationDatabase::loadFromDirectory(std::string(argv[2]),err_str);

	std::vector<std::string> files = cdb->getAllFiles();

//	//start preprocess file
//	std::vector<CompileCommand> compileCommands = cdb->getCompileCommands(std::string(argv[1]));
//	std::string compile_command = "";
//	std::string command = "";
//	std::string directory = "";
//	std::string filename = "";
//	bool oCommandFound = false;
//
//	for (unsigned i = 0; i < compileCommands.size(); i++)
//	{
//		compile_command = "";
//		directory = compileCommands[i].Directory;
//		//std::cout << directory << std::endl;
//		filename = compileCommands[i].Filename;
//		//std::cout << filename << std::endl;
//		//std::cout << compileCommands[i].Output<< std::endl;
//		oCommandFound = false;
//		for (unsigned j = 0; j < compileCommands[i].CommandLine.size(); j++)
//		{
//			if (oCommandFound == true) //skip -o value
//			{
//				oCommandFound = false;
//				continue;
//			}
//			command = compileCommands[i].CommandLine[j];
//			if (command.compare("-o") == 0) //skip -o option
//			{
//				oCommandFound = true;
//				//std::cout << "found -o command" << std::endl;
//				continue;
//			}
//			if (command.find("-I",0) == 0)
//			{
//				//std::cout << "found -I command" << std::endl;
//				//std::cout << directory + "/" + command.substr(2) << std::endl;
//				command = "-I" + directory + "/" + command.substr(2);
//			}
//			if (command.find("-L",0) == 0) {
//				//std::cout << "found -L command" << std::endl;
//				command = "-L" + directory + "/" + command.substr(2);
//			}
//
//			if (j == compileCommands[i].CommandLine.size()-1) //file name
//			{
//				command = directory + "/" + command;
//				filename = command;
//			}
//
//			compile_command = compile_command + command + " ";
//
//			//printf ("%s\n\r", compileCommands[i].CommandLine[j].c_str());
//		}
//
//		compile_command = compile_command + "-E"; //prepocess file
//
//		//printf ("%s\n\r", compile_command.c_str());
//		std::string command_to_perform = compile_command + " > " + "tmpFile.txt";
//		system(command_to_perform.c_str());
//		command_to_perform = std::string("mv ") + std::string("tmpFile.txt ") + filename;
//		system(command_to_perform.c_str());
//		//		compile_command = compile_command + compileCommands[i];
//	}
//
//	printf ("%s\n\r", compile_command.c_str());
//	//end preprocess file
	//errs() << "* " << argv[1] << "* " << "\n\r";


	for (unsigned i = 0; i < files.size(); i++)
	{
		printf ("%s\n\r", files[i].c_str());


  //check compiler version from command
  std::vector<CompileCommand> compileCommands = cdb->getCompileCommands(files[i].c_str());

  // compilerPredefines.clear();
  // if (compileCommands[0].CommandLine[0] == "cc")
  // {

  //     /*start get predefines for gcc compiler*/
  //     if (getPredefines("gcc",compilerPredefines) == 0)
  //     {
  //       std::cout << "Error while trying to get predefines for gcc\r\n" << std::endl;
  //       return 0;
  //     }
  //     /*end get predefines for gcc compiler*/
  // }
  // else if (compileCommands[0].CommandLine[0] == "c++")
  // {
  //     /*start get predefines for current compiler*/
  //     if (getPredefines("g++",compilerPredefines) == 0)
  //     {
  //       std::cout << "Error while trying to get predefines for g++\r\n" << std::endl;
  //       return 0;
  //     }
  //     /*end get predefines for current compiler*/
  // }



  // compilerSearchPaths.clear(); 
  // if (compileCommands[0].CommandLine[0] == "cc")
  // {
  //   /*start get search paths for gcc compiler*/
  //   if (getSearchPaths("gcc",compilerSearchPaths) == 0)
  //   {
  //     std::cout << "Error while trying to get search paths for gcc\r\n" << std::endl;
  //     return 0;
  //   }
  //   /*end get predefines for gcc compiler*/
  // }
  // if (compileCommands[0].CommandLine[0] == "c++")
  // {
  //   /*start get search paths for g++ compiler*/
  //   if (getSearchPaths("g++",compilerSearchPaths) == 0)
  //   {
  //     std::cout << "Error while trying to get search paths for g++\r\n" << std::endl;
  //     return 0;
  //   }
  //   /*end get predefines for g++ compiler*/
  // }


  // /*start get search paths for clang to exclude them from header search options later*/
  // clangSearchPaths.clear();
  // if (getSearchPaths("clang",clangSearchPaths) == 0)
  // {
  //   std::cout << "Error while trying to get search paths for gcc\r\n" << std::endl;
  //   return 0;
  // }
  // /*end get predefines for clang*/




		ClangTool Tool(*cdb, files.at(i));

		replacements.clear();
    // create a new Clang Tool instance (a LibTooling environment)

	//ClangTool Tool(*cdb, std::string(argv[1]));
//    ClangTool Tool(*cdb, files.at(i));
//	RefactoringTool Tool(*cdb, std::string(argv[1]));

//    std::string filePath = std::string(argv[1]);
//    if (find_string(filePath,files) == 0)
//    {
//    	return 0;
//    }

    dirPath = std::string(argv[3]);

    if (argc > 4)
    {
        // std::string rewrite_command = std::string(argv[4]);
        // if (rewrite_command.find("r") != std::string::npos)
    	   // rewrite_enable = true;
        // else
        //    rewrite_enable = false;
        rewrite_enable = true;
    }
    // run the Clang Tool, creating a new FrontendAction (explained below)
    result = Tool.run(newFrontendActionFactory<ExampleFrontendAction>().get());
    //errs() << Tool.getReplacements();
//    std::map<std::string, Replacements> &replacements =  Tool.getReplacements();
//    errs() << "replacements size = " << replacements.size() << " | \n\r";
//    for (std::map<std::string, Replacements>::iterator it = replacements.begin(); it != replacements.end();++it ) {
//    	errs() << (*it).first << " | \n\r";
//    	Replacements Rep = (*it).second;
//    	for (Replacements::const_iterator it = Rep.begin(); it != Rep.end(); ++it)
//    	{
//    		errs() << (*it).toString() << " | \n\r";
//    	}
//    }
//    errs() << "\nFound " << numFunctions << " functions.\n\n";
    // print out the rewritten source code ("rewriter" is a global var.)
    //rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(errs());
    //if (rewrite_enable)
	//	rewriter.overwriteChangedFiles();


//    for (RPLSMTS_CONTAINER_TYPE::iterator it = replacements.begin(); it != replacements.end(); ++it)
//    {
//    	errs() << "**" << (*it).first << "**" << "\n\r";
//    	std::vector<Rplsmt> rplsmtsList = (*it).second;
//    	std::sort(rplsmtsList.begin(),rplsmtsList.end(),Rplsmt::rplsmtSmallerOrEqual);
//    	for (auto i = 0; i < rplsmtsList.size(); i++)
//    	{
//    		std::cout << rplsmtsList.at(i) << "\n\r";
//    	}
//    }
    myfile.open(replacementsPath, std::ios::out|std::ios::in|std::ios::app);
    if (myfile.is_open())
    {
		for (RPLSMTS_CONTAINER_TYPE::iterator it = replacements.begin(); it != replacements.end(); ++it)
		{
			myfile << "file:" << (*it).first << "\n\r";
			std::vector<Rplsmt> rplsmtsList = (*it).second;
			std::sort(rplsmtsList.begin(),rplsmtsList.end(),Rplsmt::rplsmtSmallerOrEqual);
			for (auto i = 0; i < rplsmtsList.size(); i++)
			{
				myfile << rplsmtsList.at(i) << "\n\r";
			}
		}
        myfile.close();
    }
    else
    {
        std::cout << "Error opening file: " << replacementsPath << "\n\r";
    }
}


    //errs() << "End Tool \n\r";
    return result;
}
    
