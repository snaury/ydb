#include <library/python/ctypes/syms.h>

BEGIN_SYMS()
ESYM(clang_BlockCommandComment_getArgText)
ESYM(clang_BlockCommandComment_getCommandName)
ESYM(clang_BlockCommandComment_getNumArgs)
ESYM(clang_BlockCommandComment_getParagraph)
ESYM(clang_CXCursorSet_contains)
ESYM(clang_CXCursorSet_insert)
ESYM(clang_CXIndex_getGlobalOptions)
ESYM(clang_CXIndex_setGlobalOptions)
ESYM(clang_CXIndex_setInvocationEmissionPathOption)
ESYM(clang_CXRewriter_create)
ESYM(clang_CXRewriter_dispose)
ESYM(clang_CXRewriter_insertTextBefore)
ESYM(clang_CXRewriter_overwriteChangedFiles)
ESYM(clang_CXRewriter_removeText)
ESYM(clang_CXRewriter_replaceText)
ESYM(clang_CXRewriter_writeMainFileToStdOut)
ESYM(clang_CXXConstructor_isConvertingConstructor)
ESYM(clang_CXXConstructor_isCopyConstructor)
ESYM(clang_CXXConstructor_isDefaultConstructor)
ESYM(clang_CXXConstructor_isMoveConstructor)
ESYM(clang_CXXField_isMutable)
ESYM(clang_CXXMethod_isConst)
ESYM(clang_CXXMethod_isCopyAssignmentOperator)
ESYM(clang_CXXMethod_isDefaulted)
ESYM(clang_CXXMethod_isDeleted)
ESYM(clang_CXXMethod_isMoveAssignmentOperator)
ESYM(clang_CXXMethod_isPureVirtual)
ESYM(clang_CXXMethod_isStatic)
ESYM(clang_CXXMethod_isVirtual)
ESYM(clang_CXXRecord_isAbstract)
ESYM(clang_Comment_getChild)
ESYM(clang_Comment_getKind)
ESYM(clang_Comment_getNumChildren)
ESYM(clang_Comment_isWhitespace)
ESYM(clang_CompilationDatabase_dispose)
ESYM(clang_CompilationDatabase_fromDirectory)
ESYM(clang_CompilationDatabase_getAllCompileCommands)
ESYM(clang_CompilationDatabase_getCompileCommands)
ESYM(clang_CompileCommand_getArg)
ESYM(clang_CompileCommand_getDirectory)
ESYM(clang_CompileCommand_getFilename)
ESYM(clang_CompileCommand_getMappedSourceContent)
ESYM(clang_CompileCommand_getMappedSourcePath)
ESYM(clang_CompileCommand_getNumArgs)
ESYM(clang_CompileCommand_getNumMappedSources)
ESYM(clang_CompileCommands_dispose)
ESYM(clang_CompileCommands_getCommand)
ESYM(clang_CompileCommands_getSize)
ESYM(clang_Cursor_Evaluate)
ESYM(clang_Cursor_getArgument)
ESYM(clang_Cursor_getBriefCommentText)
ESYM(clang_Cursor_getCXXManglings)
ESYM(clang_Cursor_getCommentRange)
ESYM(clang_Cursor_getMangling)
ESYM(clang_Cursor_getModule)
ESYM(clang_Cursor_getNumArguments)
ESYM(clang_Cursor_getNumTemplateArguments)
ESYM(clang_Cursor_getObjCDeclQualifiers)
ESYM(clang_Cursor_getObjCManglings)
ESYM(clang_Cursor_getObjCPropertyAttributes)
ESYM(clang_Cursor_getObjCPropertyGetterName)
ESYM(clang_Cursor_getObjCPropertySetterName)
ESYM(clang_Cursor_getObjCSelectorIndex)
ESYM(clang_Cursor_getOffsetOfField)
ESYM(clang_Cursor_getParsedComment)
ESYM(clang_Cursor_getRawCommentText)
ESYM(clang_Cursor_getReceiverType)
ESYM(clang_Cursor_getSpellingNameRange)
ESYM(clang_Cursor_getStorageClass)
ESYM(clang_Cursor_getTemplateArgumentKind)
ESYM(clang_Cursor_getTemplateArgumentType)
ESYM(clang_Cursor_getTemplateArgumentUnsignedValue)
ESYM(clang_Cursor_getTemplateArgumentValue)
ESYM(clang_Cursor_getTranslationUnit)
ESYM(clang_Cursor_getVarDeclInitializer)
ESYM(clang_Cursor_hasAttrs)
ESYM(clang_Cursor_hasVarDeclExternalStorage)
ESYM(clang_Cursor_hasVarDeclGlobalStorage)
ESYM(clang_Cursor_isAnonymous)
ESYM(clang_Cursor_isAnonymousRecordDecl)
ESYM(clang_Cursor_isBitField)
ESYM(clang_Cursor_isDynamicCall)
ESYM(clang_Cursor_isExternalSymbol)
ESYM(clang_Cursor_isFunctionInlined)
ESYM(clang_Cursor_isInlineNamespace)
ESYM(clang_Cursor_isMacroBuiltin)
ESYM(clang_Cursor_isMacroFunctionLike)
ESYM(clang_Cursor_isNull)
ESYM(clang_Cursor_isObjCOptional)
ESYM(clang_Cursor_isVariadic)
ESYM(clang_EnumDecl_isScoped)
ESYM(clang_EvalResult_dispose)
ESYM(clang_EvalResult_getAsDouble)
ESYM(clang_EvalResult_getAsInt)
ESYM(clang_EvalResult_getAsLongLong)
ESYM(clang_EvalResult_getAsStr)
ESYM(clang_EvalResult_getAsUnsigned)
ESYM(clang_EvalResult_getKind)
ESYM(clang_EvalResult_isUnsignedInt)
ESYM(clang_File_isEqual)
ESYM(clang_File_tryGetRealPathName)
ESYM(clang_FullComment_getAsHTML)
ESYM(clang_FullComment_getAsXML)
ESYM(clang_HTMLStartTagComment_isSelfClosing)
ESYM(clang_HTMLStartTag_getAttrName)
ESYM(clang_HTMLStartTag_getAttrValue)
ESYM(clang_HTMLStartTag_getNumAttrs)
ESYM(clang_HTMLTagComment_getAsString)
ESYM(clang_HTMLTagComment_getTagName)
ESYM(clang_IndexAction_create)
ESYM(clang_IndexAction_dispose)
ESYM(clang_InlineCommandComment_getArgText)
ESYM(clang_InlineCommandComment_getCommandName)
ESYM(clang_InlineCommandComment_getNumArgs)
ESYM(clang_InlineCommandComment_getRenderKind)
ESYM(clang_InlineContentComment_hasTrailingNewline)
ESYM(clang_Location_isFromMainFile)
ESYM(clang_Location_isInSystemHeader)
ESYM(clang_ModuleMapDescriptor_create)
ESYM(clang_ModuleMapDescriptor_dispose)
ESYM(clang_ModuleMapDescriptor_setFrameworkModuleName)
ESYM(clang_ModuleMapDescriptor_setUmbrellaHeader)
ESYM(clang_ModuleMapDescriptor_writeToBuffer)
ESYM(clang_Module_getASTFile)
ESYM(clang_Module_getFullName)
ESYM(clang_Module_getName)
ESYM(clang_Module_getNumTopLevelHeaders)
ESYM(clang_Module_getParent)
ESYM(clang_Module_getTopLevelHeader)
ESYM(clang_Module_isSystem)
ESYM(clang_ParamCommandComment_getDirection)
ESYM(clang_ParamCommandComment_getParamIndex)
ESYM(clang_ParamCommandComment_getParamName)
ESYM(clang_ParamCommandComment_isDirectionExplicit)
ESYM(clang_ParamCommandComment_isParamIndexValid)
ESYM(clang_PrintingPolicy_dispose)
ESYM(clang_PrintingPolicy_getProperty)
ESYM(clang_PrintingPolicy_setProperty)
ESYM(clang_Range_isNull)
ESYM(clang_TParamCommandComment_getDepth)
ESYM(clang_TParamCommandComment_getIndex)
ESYM(clang_TParamCommandComment_getParamName)
ESYM(clang_TParamCommandComment_isParamPositionValid)
ESYM(clang_TargetInfo_dispose)
ESYM(clang_TargetInfo_getPointerWidth)
ESYM(clang_TargetInfo_getTriple)
ESYM(clang_TextComment_getText)
ESYM(clang_Type_getAlignOf)
ESYM(clang_Type_getCXXRefQualifier)
ESYM(clang_Type_getClassType)
ESYM(clang_Type_getModifiedType)
ESYM(clang_Type_getNamedType)
ESYM(clang_Type_getNullability)
ESYM(clang_Type_getNumObjCProtocolRefs)
ESYM(clang_Type_getNumObjCTypeArgs)
ESYM(clang_Type_getNumTemplateArguments)
ESYM(clang_Type_getObjCEncoding)
ESYM(clang_Type_getObjCObjectBaseType)
ESYM(clang_Type_getObjCProtocolDecl)
ESYM(clang_Type_getObjCTypeArg)
ESYM(clang_Type_getOffsetOf)
ESYM(clang_Type_getSizeOf)
ESYM(clang_Type_getTemplateArgumentAsType)
ESYM(clang_Type_getValueType)
ESYM(clang_Type_isTransparentTagTypedef)
ESYM(clang_Type_visitFields)
ESYM(clang_VerbatimBlockLineComment_getText)
ESYM(clang_VerbatimLineComment_getText)
ESYM(clang_VirtualFileOverlay_addFileMapping)
ESYM(clang_VirtualFileOverlay_create)
ESYM(clang_VirtualFileOverlay_dispose)
ESYM(clang_VirtualFileOverlay_setCaseSensitivity)
ESYM(clang_VirtualFileOverlay_writeToBuffer)
ESYM(clang_annotateTokens)
ESYM(clang_codeCompleteAt)
ESYM(clang_codeCompleteGetContainerKind)
ESYM(clang_codeCompleteGetContainerUSR)
ESYM(clang_codeCompleteGetContexts)
ESYM(clang_codeCompleteGetDiagnostic)
ESYM(clang_codeCompleteGetNumDiagnostics)
ESYM(clang_codeCompleteGetObjCSelector)
ESYM(clang_constructUSR_ObjCCategory)
ESYM(clang_constructUSR_ObjCClass)
ESYM(clang_constructUSR_ObjCIvar)
ESYM(clang_constructUSR_ObjCMethod)
ESYM(clang_constructUSR_ObjCProperty)
ESYM(clang_constructUSR_ObjCProtocol)
ESYM(clang_createAPISet)
ESYM(clang_createCXCursorSet)
ESYM(clang_createIndex)
ESYM(clang_createTranslationUnit)
ESYM(clang_createTranslationUnit2)
ESYM(clang_createTranslationUnitFromSourceFile)
ESYM(clang_defaultCodeCompleteOptions)
ESYM(clang_defaultDiagnosticDisplayOptions)
ESYM(clang_defaultEditingTranslationUnitOptions)
ESYM(clang_defaultReparseOptions)
ESYM(clang_defaultSaveOptions)
ESYM(clang_disposeAPISet)
ESYM(clang_disposeCXCursorSet)
ESYM(clang_disposeCXPlatformAvailability)
ESYM(clang_disposeCXTUResourceUsage)
ESYM(clang_disposeCodeCompleteResults)
ESYM(clang_disposeDiagnostic)
ESYM(clang_disposeDiagnosticSet)
ESYM(clang_disposeIndex)
ESYM(clang_disposeOverriddenCursors)
ESYM(clang_disposeSourceRangeList)
ESYM(clang_disposeString)
ESYM(clang_disposeStringSet)
ESYM(clang_disposeTokens)
ESYM(clang_disposeTranslationUnit)
ESYM(clang_enableStackTraces)
ESYM(clang_equalCursors)
ESYM(clang_equalLocations)
ESYM(clang_equalRanges)
ESYM(clang_equalTypes)
ESYM(clang_executeOnThread)
ESYM(clang_findIncludesInFile)
ESYM(clang_findIncludesInFileWithBlock)
ESYM(clang_findReferencesInFile)
ESYM(clang_findReferencesInFileWithBlock)
ESYM(clang_formatDiagnostic)
ESYM(clang_free)
ESYM(clang_getAddressSpace)
ESYM(clang_getAllSkippedRanges)
ESYM(clang_getArgType)
ESYM(clang_getArrayElementType)
ESYM(clang_getArraySize)
ESYM(clang_getBuildSessionTimestamp)
ESYM(clang_getCString)
ESYM(clang_getCXTUResourceUsage)
ESYM(clang_getCXXAccessSpecifier)
ESYM(clang_getCanonicalCursor)
ESYM(clang_getCanonicalType)
ESYM(clang_getChildDiagnostics)
ESYM(clang_getClangVersion)
ESYM(clang_getCompletionAnnotation)
ESYM(clang_getCompletionAvailability)
ESYM(clang_getCompletionBriefComment)
ESYM(clang_getCompletionChunkCompletionString)
ESYM(clang_getCompletionChunkKind)
ESYM(clang_getCompletionChunkText)
ESYM(clang_getCompletionFixIt)
ESYM(clang_getCompletionNumAnnotations)
ESYM(clang_getCompletionNumFixIts)
ESYM(clang_getCompletionParent)
ESYM(clang_getCompletionPriority)
ESYM(clang_getCursor)
ESYM(clang_getCursorAvailability)
ESYM(clang_getCursorCompletionString)
ESYM(clang_getCursorDefinition)
ESYM(clang_getCursorDisplayName)
ESYM(clang_getCursorExceptionSpecificationType)
ESYM(clang_getCursorExtent)
ESYM(clang_getCursorKind)
ESYM(clang_getCursorKindSpelling)
ESYM(clang_getCursorLanguage)
ESYM(clang_getCursorLexicalParent)
ESYM(clang_getCursorLinkage)
ESYM(clang_getCursorLocation)
ESYM(clang_getCursorPlatformAvailability)
ESYM(clang_getCursorPrettyPrinted)
ESYM(clang_getCursorPrintingPolicy)
ESYM(clang_getCursorReferenceNameRange)
ESYM(clang_getCursorReferenced)
ESYM(clang_getCursorResultType)
ESYM(clang_getCursorSemanticParent)
ESYM(clang_getCursorSpelling)
ESYM(clang_getCursorTLSKind)
ESYM(clang_getCursorType)
ESYM(clang_getCursorUSR)
ESYM(clang_getCursorVisibility)
ESYM(clang_getDeclObjCTypeEncoding)
ESYM(clang_getDefinitionSpellingAndExtent)
ESYM(clang_getDiagnostic)
ESYM(clang_getDiagnosticCategory)
ESYM(clang_getDiagnosticCategoryName)
ESYM(clang_getDiagnosticCategoryText)
ESYM(clang_getDiagnosticFixIt)
ESYM(clang_getDiagnosticInSet)
ESYM(clang_getDiagnosticLocation)
ESYM(clang_getDiagnosticNumFixIts)
ESYM(clang_getDiagnosticNumRanges)
ESYM(clang_getDiagnosticOption)
ESYM(clang_getDiagnosticRange)
ESYM(clang_getDiagnosticSetFromTU)
ESYM(clang_getDiagnosticSeverity)
ESYM(clang_getDiagnosticSpelling)
ESYM(clang_getElementType)
ESYM(clang_getEnumConstantDeclUnsignedValue)
ESYM(clang_getEnumConstantDeclValue)
ESYM(clang_getEnumDeclIntegerType)
ESYM(clang_getExceptionSpecificationType)
ESYM(clang_getExpansionLocation)
ESYM(clang_getFieldDeclBitWidth)
ESYM(clang_getFile)
ESYM(clang_getFileContents)
ESYM(clang_getFileLocation)
ESYM(clang_getFileName)
ESYM(clang_getFileTime)
ESYM(clang_getFileUniqueID)
ESYM(clang_getFunctionTypeCallingConv)
ESYM(clang_getIBOutletCollectionType)
ESYM(clang_getIncludedFile)
ESYM(clang_getInclusions)
ESYM(clang_getInstantiationLocation)
ESYM(clang_getLocation)
ESYM(clang_getLocationForOffset)
ESYM(clang_getModuleForFile)
ESYM(clang_getNonReferenceType)
ESYM(clang_getNullCursor)
ESYM(clang_getNullLocation)
ESYM(clang_getNullRange)
ESYM(clang_getNumArgTypes)
ESYM(clang_getNumCompletionChunks)
ESYM(clang_getNumDiagnostics)
ESYM(clang_getNumDiagnosticsInSet)
ESYM(clang_getNumElements)
ESYM(clang_getNumOverloadedDecls)
ESYM(clang_getOverloadedDecl)
ESYM(clang_getOverriddenCursors)
ESYM(clang_getPointeeType)
ESYM(clang_getPresumedLocation)
ESYM(clang_getRange)
ESYM(clang_getRangeEnd)
ESYM(clang_getRangeStart)
ESYM(clang_getRemappings)
ESYM(clang_getRemappingsFromFileList)
ESYM(clang_getResultType)
ESYM(clang_getSkippedRanges)
ESYM(clang_getSpecializedCursorTemplate)
ESYM(clang_getSpellingLocation)
ESYM(clang_getSymbolGraphForCursor)
ESYM(clang_getSymbolGraphForUSR)
ESYM(clang_getTUResourceUsageName)
ESYM(clang_getTemplateCursorKind)
ESYM(clang_getToken)
ESYM(clang_getTokenExtent)
ESYM(clang_getTokenKind)
ESYM(clang_getTokenLocation)
ESYM(clang_getTokenSpelling)
ESYM(clang_getTranslationUnitCursor)
ESYM(clang_getTranslationUnitSpelling)
ESYM(clang_getTranslationUnitTargetInfo)
ESYM(clang_getTypeDeclaration)
ESYM(clang_getTypeKindSpelling)
ESYM(clang_getTypeSpelling)
ESYM(clang_getTypedefDeclUnderlyingType)
ESYM(clang_getTypedefName)
ESYM(clang_getUnqualifiedType)
ESYM(clang_hashCursor)
ESYM(clang_indexLoc_getCXSourceLocation)
ESYM(clang_indexLoc_getFileLocation)
ESYM(clang_indexSourceFile)
ESYM(clang_indexSourceFileFullArgv)
ESYM(clang_indexTranslationUnit)
ESYM(clang_index_getCXXClassDeclInfo)
ESYM(clang_index_getClientContainer)
ESYM(clang_index_getClientEntity)
ESYM(clang_index_getIBOutletCollectionAttrInfo)
ESYM(clang_index_getObjCCategoryDeclInfo)
ESYM(clang_index_getObjCContainerDeclInfo)
ESYM(clang_index_getObjCInterfaceDeclInfo)
ESYM(clang_index_getObjCPropertyDeclInfo)
ESYM(clang_index_getObjCProtocolRefListInfo)
ESYM(clang_index_isEntityObjCContainerKind)
ESYM(clang_index_setClientContainer)
ESYM(clang_index_setClientEntity)
ESYM(clang_install_aborting_llvm_fatal_error_handler)
ESYM(clang_isAttribute)
ESYM(clang_isConstQualifiedType)
ESYM(clang_isCursorDefinition)
ESYM(clang_isDeclaration)
ESYM(clang_isExpression)
ESYM(clang_isFileMultipleIncludeGuarded)
ESYM(clang_isFunctionTypeVariadic)
ESYM(clang_isInvalid)
ESYM(clang_isInvalidDeclaration)
ESYM(clang_isPODType)
ESYM(clang_isPreprocessing)
ESYM(clang_isReference)
ESYM(clang_isRestrictQualifiedType)
ESYM(clang_isStatement)
ESYM(clang_isTranslationUnit)
ESYM(clang_isUnexposed)
ESYM(clang_isVirtualBase)
ESYM(clang_isVolatileQualifiedType)
ESYM(clang_loadDiagnostics)
ESYM(clang_parseTranslationUnit)
ESYM(clang_parseTranslationUnit2)
ESYM(clang_parseTranslationUnit2FullArgv)
ESYM(clang_remap_dispose)
ESYM(clang_remap_getFilenames)
ESYM(clang_remap_getNumFiles)
ESYM(clang_reparseTranslationUnit)
ESYM(clang_saveTranslationUnit)
ESYM(clang_sortCodeCompletionResults)
ESYM(clang_suspendTranslationUnit)
ESYM(clang_toggleCrashRecovery)
ESYM(clang_tokenize)
ESYM(clang_uninstall_llvm_fatal_error_handler)
ESYM(clang_visitChildren)
ESYM(clang_visitChildrenWithBlock)
END_SYMS()
