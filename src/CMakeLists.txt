
set(LIBRARY_HEADERS
    Token.hpp
    lexer.hpp
    repl.hpp
    ast.hpp
    parser.hpp
    object.hpp
    evaluator.hpp
    globals.hpp
    environment.hpp
    macros.hpp
    SyntaxParser.hpp
    ASTBuilder.hpp
    TByteCodeBuilder.hpp
    TListObject.hpp
    TStringObject.hpp
    ASTNodeTypes.hpp
    ConstantTable.hpp
    MachineStack.hpp
    TokenTable.hpp
    TSymbolTable.hpp
    MemoryManager.hpp
    ASTNode.hpp
)

set(LIBRARY_SOURCES
    Token.cpp
    lexer.cpp
    repl.cpp
    ast.cpp
    parser.cpp
    object.cpp
    evaluator.cpp
    globals.cpp
    environment.cpp
    macros.cpp
    SyntaxParser.cpp
    TokenTable.cpp
    ASTBuilder.cpp
    ASTNode.cpp
    VM.hpp
    VM.cpp
    TSymbolTable.cpp
    OpCodes.hpp
    OpCodes.cpp
    MachineStack.cpp
    ConstantTable.cpp
    MemoryManager.cpp
    TListObject.cpp
    TByteCodeBuilder.cpp
)

add_library(${LIBRARY_NAME} STATIC ${LIBRARY_SOURCES} ${LIBRARY_HEADERS})
target_include_directories(${LIBRARY_NAME} PUBLIC "./")

if(${ENABLE_LTO})
    target_enable_lto(
        TARGET
        ${LIBRARY_NAME}
        ENABLE
        ON)
endif()

if (${ENABLE_CLANG_TIDY})
    add_clang_tidy_to_target(${LIBRARY_NAME})
endif()