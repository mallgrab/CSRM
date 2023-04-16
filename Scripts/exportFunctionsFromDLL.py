from typing import List
import subprocess
import sys
import os
import re

class Functions:
    exportedName: str = ""
    formattedName: str = ""
    typeDef: str = ""
    pointer: str = ""
    extern: str = ""
    definition: str = ""
    declaration: str = ""
    returnType: str = ""
    defType: str = ""
    flagRemove: bool = False

    def __init__(self, exported, formated):
        self.exportedName = exported
        self.formattedName = formated

def main():
    currentDirectory = os.getcwd()
    dllName: str = ""
    fitlerByString: str = ""

    if len(sys.argv) >= 2:
        dllName = sys.argv[1]

    if len(sys.argv) >= 3:
        fitlerByString = sys.argv[2]

    if len(sys.argv) == 1:
        print("exportFunctionsFromDLL.py dllName className(optional)")
        return
        
    # get output from dumpbin
    cmd = "dumpbin /EXPORTS " + dllName

    try:
        output = subprocess.check_output(cmd, text=True)
    except FileNotFoundError:
        print("dumpbin does not exist in your user environment")
        return

    outputList = output.split('\n')
    
    # find start index from output of exported functions
    listStartIndex = 0
    for index, string in enumerate(outputList):
        if string.find("ordinal") != -1:
            listStartIndex = index

    listStartIndex += 2

    # remove elements at the top of the list until we are at the first exported function
    for _ in range(0, listStartIndex):
        outputList.pop(0)

    # find end index from output of exported functions
    listStopIndex = 0
    for index, string in enumerate(outputList):
        if string.find("Summary") != -1:
            listStopIndex = index

    amountOfLinesToDeleteAtEndOfList = len(outputList) - listStopIndex + 1

    # remove bottom list elements until we are at the last exported function
    for _ in range(0, amountOfLinesToDeleteAtEndOfList):
        outputList.pop()

    # remove address, function number from string so we only get the exported function name
    for index, string in enumerate(outputList):
        exportedString = string.split()
        outputList[index] = exportedString[3]

    formattedFunctionList = list()
    exportedFunctionList = list()
    for index, string in enumerate(outputList):
        if fitlerByString:
            if string.find(fitlerByString) == -1:
                continue
        
        exportedFunctionList.append(string)
        functionName = re.sub('@@', ' ', string)
        if (functionName != string):
            functionName = functionName.split()
            functionName = functionName[0]
        else:
            print("Error: Exported function didn't have double @ symbols in name!")
            raise
        formattedFunctionList.append(functionName)

    # create a list that stores both the exported function name & the formatted name
    functionList:List[Functions] = list()
    suffixCounterConstructor = 1
    counterConstructorName = ""
    suffixCounterFunction = 1

    # add exported function strings for later demangling
    stringToDemangle: str = ""

    for index, string in enumerate(formattedFunctionList):
        stringToFormat: str = ''
        stringToFormat = formattedFunctionList[index]
        stringToFormat = stringToFormat.replace('?', '')
        stringToFormat = re.sub('\\d', '', stringToFormat)
        amountAtSymbols = stringToFormat.count('@')

        if amountAtSymbols == 1:
            stringToFormat = re.sub('@', ' ', stringToFormat)
            constructorName = stringToFormat.split()
            constructorName = constructorName[0]

            # add a suffix counter if multiple constructors exist
            for function in functionList:
                if function.formattedName == constructorName:
                    if counterConstructorName != constructorName:
                        counterConstructorName = constructorName
                        suffixCounterConstructor = 1

                    constructorName += str(suffixCounterConstructor)
                    suffixCounterConstructor += 1
            
            stringToDemangle += exportedFunctionList[index] + " "
            functionList.append(Functions(exportedFunctionList[index], constructorName))
        else:
            # dont include functions with namespaces if we didnt search for one
            # this is very unreliable and still includes functions with namespaces/classes
            # since the data we get from dumpbin isnt sofisticated enough to differentiate
            # exported functions with only the amount of @ symbols
            # instead we should use data from ida/ghidra since those tools can demangle the names
            # of the exported functions
            if not fitlerByString:
                continue

            stringToFormat = re.sub('@', ' ', stringToFormat)
            functionName = stringToFormat.split()
        
            # dont include functions that dont have the same namespace that we are searching for
            if fitlerByString:
                namespaceFromFunction = functionName[1]
                if namespaceFromFunction != fitlerByString:
                    continue

            functionName = functionName[0]

            for function in functionList:
                if function.formattedName == functionName:
                    if counterConstructorName != functionName:
                        counterConstructorName = functionName
                        suffixCounterFunction = 1

                    functionName += str(suffixCounterFunction)
                    suffixCounterFunction += 1

            stringToDemangle += exportedFunctionList[index] + " "
            functionList.append(Functions(exportedFunctionList[index], functionName))

    # get demangled strings into a list
    demangledStringOutput: list = list()
    cmd = "demumble " + stringToDemangle

    try:
        output = subprocess.check_output(cmd, text=True)
    except FileNotFoundError:
        print("dumpbin does not exist in your user environment")
        return

    demangledStringOutput = output.split('\n')

    # remove last element in the list if it got included as an element from split as an empty newline
    if len(demangledStringOutput[len(demangledStringOutput)-1]) == 0:
        demangledStringOutput.pop()

    # get demangled return type & arguments from exported function
    for index, function in enumerate(functionList):
        demangledFunctionDefinition = demangledStringOutput[index].split(" ")
        
        # normally the second element in the list is the return type
        # on constructors it usually doesnt show what type it returns
        # could be a pointer tho... check in ghidra/ida
        if demangledFunctionDefinition[1] == '__cdecl':
            function.returnType = "void"
        elif demangledFunctionDefinition[1] == 'enum':
            function.returnType = "int64_t /*enum*/"
        elif demangledFunctionDefinition[1] == 'static' or demangledFunctionDefinition[1] == 'class':
            function.returnType = demangledFunctionDefinition[2]
        else:
            function.returnType = demangledFunctionDefinition[1]

        if function.returnType == 'class':
            function.returnType = "void*"

        # get only the arguments from the function
        demangledFunctionArguments = demangledStringOutput[index].split("(")[1]
        demangledFunctionArguments = demangledFunctionArguments.split(")")[0]

        # remove class and enum from the argument types
        demangledFunctionArguments = demangledFunctionArguments.replace("class ", "")
        demangledFunctionArguments = demangledFunctionArguments.replace("enum ", "")
        demangledFunctionArguments = demangledFunctionArguments.replace(" const &", "")
        demangledFunctionArguments = demangledFunctionArguments.replace(" const", "")
        demangledFunctionArguments = demangledFunctionArguments.replace(" &", "")
        
        variableNameCounter: int = 1
        splitByEachArgument = demangledFunctionArguments.split(",")
        for index, string in enumerate(splitByEachArgument):
            if string[0] == ' ':
                splitByEachArgument[index] = string[1:]

        for index, string in enumerate(splitByEachArgument):
            # dont add an additional space character if the variable is a pointer
            if splitByEachArgument[index][-1:] == '*':
                splitByEachArgument[index] = splitByEachArgument[index] + "a" + str(variableNameCounter)
            else:
                splitByEachArgument[index] = splitByEachArgument[index] + " a" + str(variableNameCounter)
            variableNameCounter += 1
        
        defType: str = ""

        for index, string in enumerate(splitByEachArgument):
            if len(splitByEachArgument) - 1 != index:
                defType += string + ", "
            else:
                defType += string

        function.defType = defType

        if demangledFunctionArguments == "void":
            function.defType = ""

    regularFunctionList:List[Functions] = functionList.copy()
    overloadedFunctionList: list = list()
    overloaded: bool = False

    # move overloaded functions into their own list so we can handle them seperately
    for index in range(0, len(functionList)):
        if len(functionList) == 0:
            break
        
        overloadedList: list = list()
        function = functionList[0]
        functionList.pop(0)

        for _ in functionList:
            searchFunction = functionList[0]

            # since we add a suffix counter to child functions we need to remove it
            parentFormattedName = re.sub('\\d', '', function.formattedName)
            childFormattedName = re.sub('\\d', '', searchFunction.formattedName)

            # check if the child function and the parent function are the same
            # if they are add them to the overloaded list + the parent function
            if parentFormattedName == childFormattedName:
                overloaded = True
                overloadedList.append(searchFunction)
                functionList.pop(0)

        if overloaded:
            overloadedList.insert(0, function)

            # flag overloaded functions inside regular function list for removal
            for regularFunction in regularFunctionList:
                for overloadedFunction in overloadedList:
                    if regularFunction.formattedName == overloadedFunction.formattedName:
                        regularFunction.flagRemove = True
                        break

            overloadedFunctionList.append(overloadedList)
            overloaded = False

    # remove flagged functions that are overloaded
    copyList = regularFunctionList.copy()
    for function in copyList:
        if function.flagRemove:
            regularFunctionList.remove(function)
    
    # construct string output from exported & named functions
    for function in regularFunctionList:
        function.typeDef = " ".join(["using", function.formattedName + "_t", "=", function.returnType, "(__fastcall*)(" + function.defType + ");"])
        function.extern = " ".join(["extern", function.formattedName + "_t", function.formattedName + ";"])
        function.definition = " ".join([function.formattedName + "_t", function.formattedName])
        function.pointer = " ".join([function.formattedName, "=", "(" + function.formattedName + "_t" + ")" + 'GetProcAddress(module, "' + function.exportedName + '");'])

    for functionList in overloadedFunctionList:
        for function in functionList:
            function.typeDef = " ".join(["using", function.formattedName + "_t", "=", function.returnType, "(__fastcall*)(" + function.defType + ");"])
            function.extern = " ".join(["extern", function.formattedName + "_t", function.formattedName + ";"])
            function.definition = " ".join([function.formattedName + "_t", function.formattedName])

            # since we are suffixing a number to the formatted name, remove it again
            functionNameFormatted = re.sub('\\d', '', function.formattedName)
            function.declaration = " ".join([function.returnType, functionNameFormatted + "(" + function.defType + ")"])

            # suffix with func so we dont get naming collision
            function.pointer = " ".join([function.formattedName + "Func", "=", "(" + function.formattedName + "_t" + ")" + 'GetProcAddress(module, "' + function.exportedName + '");'])

    # write out the header file
    fileOutputHeader = open(currentDirectory + "\\output.h", "w")
    fileOutputHeader.write("#pragma once\n\n")

    for function in regularFunctionList:
        fileOutputHeader.write(function.typeDef + "\n")

    fileOutputHeader.write("\n")

    for function in regularFunctionList:
        fileOutputHeader.write(function.extern + "\n")

    fileOutputHeader.write("\n")

    for functionList in overloadedFunctionList:
        for function in functionList:
            fileOutputHeader.write(function.typeDef + "\n")
            fileOutputHeader.write(function.declaration + ";" + "\n")
            fileOutputHeader.write("\n")

    fileOutputHeader.write("void " + fitlerByString + "InstallHooks(LPCWSTR dllName);")

    # write out the class file
    fileOutputClass = open(currentDirectory + "\\output.cpp", "w")
    fileOutputClass.write('#include "output.h"' + "\n\n")

    for function in regularFunctionList:
        fileOutputClass.write(function.definition + ";" + "\n")

    for functionList in overloadedFunctionList:
        for function in functionList:
            # suffix with func so we dont get naming collision
            fileOutputClass.write(function.definition + "Func" + ";" + "\n")

    for functionList in overloadedFunctionList:
        for function in functionList:
            fileOutputClass.write("\n")
            fileOutputClass.write(function.declaration + "\n")
            fileOutputClass.write("{" + "\n")
            # suffix with func so we dont get naming collision
            suffixedFormattedName = function.formattedName + "Func"

            # check if we need to return the result of the original function call
            if function.returnType == 'void':
                fileOutputClass.write("\t" + suffixedFormattedName + "(" + function.defType + ");" + "\n")
            else:
                fileOutputClass.write("\t return " + suffixedFormattedName + "(" + function.defType + ");" + "\n")

            fileOutputClass.write("}" + "\n")

    fileOutputClass.write("\n" + "void " + fitlerByString + "InstallHooks(LPCWSTR dllName)" + "\n")
    fileOutputClass.write("{" + "\n")
    fileOutputClass.write("\t" + "HINSTANCE module = GetModuleHandle(dllName);" + "\n")
    fileOutputClass.write("\t" + "\n")
    fileOutputClass.write("\t" + "if (module != NULL)" + "\n")
    fileOutputClass.write("\t" + "{" + "\n")

    for function in regularFunctionList:
        fileOutputClass.write("\t\t" + function.pointer + "\n")

    for functionList in overloadedFunctionList:
        for function in functionList:
            fileOutputClass.write("\t\t" + function.pointer + "\n")

    fileOutputClass.write("\t" + "}" + "\n")
    fileOutputClass.write("}" + "\n")

    # close files, we are finish
    fileOutputClass.close()
    fileOutputHeader.close()
    
    print("Finished, exported " + str(len(exportedFunctionList)) + " functions.")

if __name__ == '__main__':
    sys.exit(main())