/* llvm-c/Target.h helper functions wrappers.
 *
 * The LLVMInitializeAll* functions and friends are defined `static inline`, so
 * we can't bind directly to them (the function body is generated via macro),
 * so here are some wrappers.
 */

#include "llvm-c/Core.h"
#include <llvm-c/Target.h>
#include "llvm/CodeGen/CommandFlags.h"
#include "llvm/IR/IRPrintingPasses.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Support/WithColor.h"
#include <llvm/Target/TargetMachine.h>
#include <llvm/Transforms/Utils/BasicBlockUtils.h>
#include <llvm/MC/TargetRegistry.h>
#include <iostream>
// #include <stdio.h>

extern "C"
{
    using namespace llvm;
    void LLVM_InitializeAllTargetInfos(void)
    {
        InitializeAllTargetInfos();
        LLVMInitializeAllTargetInfos();
    }

    void LLVM_InitializeAllTargets(void)
    {
        InitializeAllTargets();
        LLVMInitializeAllTargets();
    }

    void LLVM_InitializeAllTargetMCs(void)
    {
        InitializeAllTargetMCs();
        LLVMInitializeAllTargetMCs();
    }

    void LLVM_InitializeAllAsmPrinters(void)
    {
        InitializeAllAsmPrinters();
        LLVMInitializeAllAsmPrinters();
    }

    void LLVM_InitializeAllAsmParsers(void)
    {
        InitializeAllAsmParsers();
        LLVMInitializeAllAsmParsers();
    }

    void LLVM_InitializeAllDisassemblers(void)
    {
        InitializeAllDisassemblers();
        LLVMInitializeAllDisassemblers();
    }

    /* These functions return true on failure. */
    LLVMBool LLVM_InitializeNativeTarget(void)
    {
        return LLVMInitializeNativeTarget();
    }

    LLVMBool LLVM_InitializeNativeAsmParser(void)
    {
        return LLVMInitializeNativeAsmParser();
    }

    LLVMBool LLVM_InitializeNativeAsmPrinter(void)
    {
        return LLVMInitializeNativeAsmPrinter();
    }

    LLVMBool LLVM_InitializeNativeDisassembler(void)
    {
        return LLVMInitializeNativeDisassembler();
    }

    LLVMValueRef LLVMGetInsertPoint(LLVMBasicBlockRef bb)
    {
        auto block = unwrap(bb);
        IRBuilder<> ir(block);
        auto point = ir.saveIP();
        auto poitj = new IRBuilderBase::InsertPoint(point);
        return (LLVMValueRef)poitj;
    }

    void LLVMSetInsertPoint(LLVMBasicBlockRef bb, LLVMValueRef ip)
    {
        auto block = unwrap(bb);
        IRBuilder<> ir(block);
        ir.SetInsertPoint(block);
        auto point = (IRBuilderBase::InsertPoint *)ip;
        ir.restoreIP(*point);
        delete point;
    }

    uint64_t LLVMGetTypeSize(LLVMModuleRef M, LLVMTypeRef ty)
    {
        auto mod = unwrap(M);
        auto type = unwrap(ty);
        DataLayout dl(mod);
        auto size = dl.getTypeAllocSize(type);
        return size.getFixedSize();
    }

    void LLVMSetAllocatedType(LLVMValueRef Alloca, LLVMModuleRef M, LLVMTypeRef ty)
    {
        auto mod = unwrap(M);
        auto type = unwrap(ty);
        auto alloc = unwrap<AllocaInst>(Alloca);
        auto alloc1 = unwrap(Alloca);

        alloc->setAllocatedType(type);
        alloc1->mutateType(type->getPointerTo());

        DataLayout dl(mod);
        alloc->setAlignment(dl.getPrefTypeAlign(type));
    }

    LLVMValueRef LLVMGetValueAt(LLVMValueRef r, uint32_t index)
    {
        auto p = unwrap<ConstantDataArray>(r);
        auto con = p->getElementAsConstant(index);

        return wrap(con);
    }

    LLVMValueRef LLVMCreateStringLiteral(LLVMBuilderRef B, const char *Str, unsigned Length, const char *Name)
    {
        return wrap(unwrap(B)->CreateGlobalString(StringRef(Str, Length), Name));
    }

    LLVMValueRef LLVMCreateGlobalValue(LLVMModuleRef M, LLVMTypeRef ty, LLVMValueRef val)
    {
        auto mod= unwrap(M);
        auto *GV = new GlobalVariable(
            *mod, unwrap(ty), true, GlobalValue::PrivateLinkage,
            unwrap<Constant>(val), "", nullptr, GlobalVariable::NotThreadLocal, 0);
        GV->setUnnamedAddr(GlobalValue::UnnamedAddr::Global);
        GV->setAlignment(Align(1));
        return wrap(GV);
    }

    size_t LLVMNumberOfChildrenInBlock(LLVMBasicBlockRef B)
    {
        return unwrap(B)->size();
    }

    LLVMValueRef LLVMBuildAlignedLoad(LLVMBuilderRef B, LLVMTypeRef Ty,
                                      LLVMValueRef PointerVal, uint32_t align, const char *Name)
    {
        return wrap(unwrap(B)->CreateAlignedLoad(unwrap(Ty), unwrap(PointerVal), MaybeAlign(align), Name));
    }
}
