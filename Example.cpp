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

#include "clang/Lex/Preprocessor.h"
#include "clang/Lex/PreprocessorOptions.h"
#include "clang/Tooling/CompilationDatabase.h"
#include "llvm/Support/Path.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>

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

#define DEBUG_PRINT_EXAMPLE 1

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
                filePath = MakeAbsolutePath(rewriter.getSourceMgr(),rewriter.getSourceMgr().getFilename(func->getBody()->getLocStart()));

//                idString = filePath + " " + func->getQualifiedNameAsString() + " ";

//                for (unsigned int i = 0; i < funcParams.size(); i++)
//                {
//                	funcParamsString += funcParams[i]->getQualifiedNameAsString() + " ";
//                }
//
//                idString += funcParamsString + " ";
//                idString += funcRetType.getAsString() + " ";



                //file_processed = find_string(idString, processed_files);
                SourceLocation startPattern = func->getBody()->getLocStart();
//                SourceLocation endPattern = func->getBody()->getLocStart().getLocWithOffset(10);
                SourceLocation endPattern = func->getBody()->getLocStart().getLocWithOffset(14);
                CharSourceRange charSourceRange = clang::CharSourceRange::getCharRange(startPattern,endPattern);
                std::string patternLine = Lexer::getSourceText(charSourceRange,rewriter.getSourceMgr(),rewriter.getLangOpts());

//                DEBUG_PRINT (std::string("pattern "  + patternLine + "\n\r").c_str());
                if (patternLine.find("DEFBEG") != std::string::npos)
                {
                	file_processed = 1;
                }


                //std::cout << filePath << "**\n" ;
                if ((filePath.find(dirPath) == 0) && (file_processed == 0)) //is in directory and not processed yet
                {
//                	DEBUG_PRINT (std::string("rw "  + filePath + "\n\r").c_str());
                    braceLoc = func->getBody()->getLocStart().getLocWithOffset(1);            
                    if (braceLoc.isValid())
                    {
                    		//rewriter.InsertTextAfter(braceLoc, " /*DEFBEG_777*/ ");
                    		DEBUG_PRINT (std::string(filePath + " B " + std::to_string(sm.getFileOffset(sm.getSpellingLoc(braceLoc))) + "\n\r").c_str());
                    }
                    else
                    	DEBUG_PRINT ("braceLoc is invalid=\n\r");
//                    if (sm.isInMainFile(func->getDefinition()->getBodyRBrace()))
                        //rewriter.InsertTextAfter(func->getDefinition()->getBodyRbrace(), " DEFEND_777 ");
					//rewriter.InsertTextAfter(func->getBody()->getLocEnd(), " /*DEFEND_777*/ ");
					DEBUG_PRINT (std::string(filePath + " E " + std::to_string(sm.getFileOffset(sm.getSpellingLoc(func->getBody()->getLocEnd()))) + "\n\r").c_str());
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
//        	 if (retExpr != NULL)
//        	 {
//        		 retExprEndLoc = sm.getSpellingLoc(retExpr->getLocEnd());
//        	 }
//        	 else
//        	 {
//        		 retExprEndLoc = sm.getSpellingLoc(ret->getLocEnd());
//        	 }

        	 retExprEndLoc = sm.getSpellingLoc(ret->getLocEnd());

        	 int  file_processed = 0;
        	 SourceLocation startPattern = sm.getSpellingLoc(ret->getLocEnd()).getLocWithOffset(10);
//             SourceLocation startPattern = sm.getSpellingLoc(ret->getLocStart()).getLocWithOffset(-6);
//             SourceLocation startPattern = sm.getSpellingLoc(ret->getLocStart()).getLocWithOffset(-10);
			 SourceLocation endPattern = sm.getSpellingLoc(ret->getLocStart());
			 CharSourceRange charSourceRange = clang::CharSourceRange::getCharRange(endPattern,startPattern);
//			 CharSourceRange charSourceRange = clang::CharSourceRange::getCharRange(startPattern,endPattern);

			 std::string patternLine = Lexer::getSourceText(charSourceRange,rewriter.getSourceMgr(),rewriter.getLangOpts());

			 //SourceRange sourceRange = clang::SourceRange(endPattern,startPattern);
			 //std::string patternLine = rewriter.getRewrittenText(sourceRange);

//			 DEBUG_PRINT (std::string("pattern "  + patternLine + "\n\r").c_str());

			 if (patternLine.find("DRS7") != std::string::npos)
			 {
				file_processed = 1;
			 }


        	 //SourceLocation returnSemiLoc  = sm.getSpellingLoc(Lexer::findLocationAfterToken(retExprEndLoc,clang::tok::TokenKind::semi,rewriter.getSourceMgr(), rewriter.getLangOpts(),false));
        	 SourceLocation returnSemiLoc  = sm.getSpellingLoc(ret->getLocStart().getLocWithOffset(6));

        	 if (returnSemiLoc.isValid() && (file_processed == 0) && (filePath.find(dirPath) == 0))
        	 {
//        		 rewriter.InsertTextAfter(sm.getSpellingLoc(ret->getLocStart()), "/*DRS7*/ ");
//        		 rewriter.InsertTextBefore(returnSemiLoc, " /*DRE7*/ ");
        		 DEBUG_PRINT (std::string(filePath + " R " + std::to_string(sm.getFileOffset(sm.getSpellingLoc(returnSemiLoc))) + "\n\r").c_str());
        		 //rewriter.InsertTextAfter(sm.getSpellingLoc(returnSemiLoc), " /*DRS7*/ ");
//        		         		 rewriter.InsertTextBefore(returnSemiLoc, "");
//        		 if (rewrite_enable) rewriter.overwriteChangedFiles();
        	 }

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

//    virtual bool VisitCallExpr(CallExpr *call) {
//
//		clang::SourceManager &sm = rewriter.getSourceMgr();
//        SourceLocation callStart = sm.getSpellingLoc(call->getLocStart());
//        SourceLocation callEnd = sm.getSpellingLoc(call->getLocEnd());
//
//        SourceLocation returnSemiLoc  = sm.getSpellingLoc(Lexer::findLocationAfterToken(callEnd,clang::tok::TokenKind::semi ,rewriter.getSourceMgr(), rewriter.getLangOpts(),false));
//        if (returnSemiLoc.isValid())
//        {
//        	CharSourceRange charSourceRange = clang::CharSourceRange::getCharRange(callStart,returnSemiLoc);
//        	std::string patternLine = Lexer::getSourceText(charSourceRange,rewriter.getSourceMgr(),rewriter.getLangOpts());
//
//        	std::replace(patternLine.begin(),patternLine.end(), '"','\'');
//
//        	std::string startPrint = "printf (\"call " + patternLine + " \\n\\r\");";
//        	std::string endPrint = "printf (\"after " + patternLine + " \\n\\r\");";
//
//         	rewriter.InsertTextAfter(callStart, startPrint);
//            rewriter.InsertTextBefore(returnSemiLoc, endPrint);
//        }
//
//
//
//        //errs() << "** Rewrote function call\n";
//        return true;
//    }

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
        return (std::unique_ptr<ASTConsumer>)(new ExampleASTConsumer(&CI)); // pass CI pointer to ASTConsumer
    }

    ~ExampleFrontendAction()
    {
    	//if (rewrite_enable) rewriter.overwriteChangedFiles();
    }


    virtual bool BeginSourceFileAction   (CompilerInstance &  CI)
    {
        // clang::PreprocessorOptions &po = CI.getPreprocessorOpts();
        // po.addMacroDef(llvm::StringRef("DEFBEG_777=\"\""));
        clang::Preprocessor &pp = CI.getPreprocessor();
        std::string predefines = pp.getPredefines();
        predefines.append("#define DEFBEG_777 \n");
        predefines.append("#define DEFEND_777 \n");
        predefines.append("#define DRS7  \n");
        predefines.append("#define DRE7  \n");
        pp.setPredefines(predefines);
        //std::cout << pp.getPredefines() << " |||\n";

        return true;
    }

    virtual void EndSourceFileAction   ()
    {
        // clang::PreprocessorOptions &po = CI.getPreprocessorOpts();
        // po.addMacroDef(llvm::StringRef("DEFBEG_777=\"\""));
    	SourceManager &SM = rewriter.getSourceMgr();
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

//	for (unsigned i = 0; i < files.size(); i++)
//	{
//		printf ("%s\n\r", files[i].c_str());
//	}

    // create a new Clang Tool instance (a LibTooling environment)
    errs() << "* " << argv[1] << "* " << "\n\r";
	ClangTool Tool(*cdb, std::string(argv[1]));
//    ClangTool Tool(*cdb, files.at(i));
//	RefactoringTool Tool(*cdb, std::string(argv[1]));

    std::string filePath = std::string(argv[1]);
    if (find_string(filePath,files) == 0)
    {
    	return 0;
    }



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
    errs() << "End Tool \n\r";
    return result;
}
    
