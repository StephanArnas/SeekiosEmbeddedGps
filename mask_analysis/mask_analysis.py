from os import listdir
from os.path import isfile, isdir, join
import glob
import re


#TODO : on peut l'améliorer simplement : ne pas se soucier de si hasParam ou hasReturn, simplement regarder le type de la fonction, et recherger si il est présent sur la ligne (1 ou 2 fois ?)

class MaskFunction(object):
	name = ""
	type = "" #request, running, alarm, interrupt
	hasReturn = 0 #yes if it has uint32_t/EventBits_t as param
	hasParam = 0 #yes if it has uint32_t/EventBits as return value
	
	def __init__ (self, name, type, hasReturn, hasParam):
		self.name = name
		self.type = type
		self.hasReturn = hasReturn
		self.hasParam = hasParam
		
	def __repr__ (self):
		return "Name : {} - Type : {} -  HasReturn : {} - HasParam : {}\r\n".format(self.name, self.type, self.hasReturn, self.hasParam)
		
def parseMaskFunctions(maskFunctionList):
	fileName = "functionList.txt"
	with open(fileName, 'rt', encoding="utf-8", errors='replace') as input:
		for line in input:
			hasReturn = 0
			hasParam = 0
			spaceSplit = line.split(' ')
			#print(spaceSplit)
			if spaceSplit[0] == 'EventBits_t' or spaceSplit[0] == 'uint32_t' :
				hasReturn = 1
			parenthesisSplit = spaceSplit[1].replace('(',')').split(')')
			#print(parenthesisSplit)
			name = parenthesisSplit[0]
			type=''
			if name.lower().find('running') > -1:
				type = 'running'
			if name.lower().find('request') > -1:
				type = 'request'
			if name.lower().find('alarm') > -1:
				type = 'alarm'
			if name.lower().find('interrupt') > -1:
				type = 'interrupt'
			if parenthesisSplit[1] == 'EventBits_t' or parenthesisSplit[1] == 'uint32_t' :
				hasParam = 1
			maskFunction = MaskFunction(name, type, hasReturn, hasParam)
			maskFunctionList.append(maskFunction)

def getReturnType(line):
	equalSplit = line.split("=")
	if len(equalSplit) > 1:
		if equalSplit[0].lower().find('running') > -1:
			return 'running'
		if equalSplit[0].lower().find('request') > -1:
			return 'request'
		if equalSplit[0].lower().find('alarm') > -1:
			return'alarm'
		if equalSplit[0].lower().find('interrupt') > -1:
			return 'interrupt'
	return ''


def getParamType(line):
	equalSplit = line.split("=")
	functionIndex = 0
	equalSplitIndex = 0
	if len(equalSplit) > 1:
		equalSplitIndex = 1
	parenthesisSplit = equalSplit[equalSplitIndex].replace('(',')').split(')')
	if len(parenthesisSplit) > 1:
		#print(parenthesisSplit[1])
		if parenthesisSplit[1].lower().find('running') > -1:
			return 'running'
		if parenthesisSplit[1].lower().find('request') > -1:
			return 'request'
		if parenthesisSplit[1].lower().find('alarm') > -1:
			return 'alarm'
		if parenthesisSplit[1].lower().find('interrupt') > -1:
			return 'interrupt'
	return ''
	
def checkMaskFunctionUse(maskFunctionList, pathFiles):
	errors = 0
	for file in pathFiles:
		if file.find(".c") > -1 :
			with open(file, 'rt', encoding="utf-8", errors='replace') as input:
				lineNum = 0
				for line in input:
					lineNum = lineNum + 1
					for maskFunction in maskFunctionList:
						if line.find(maskFunction.name) > -1:
							if maskFunction.hasReturn:
								returnType = getReturnType(line)
								if returnType != maskFunction.type and returnType != "":
									errors = errors + 1
									print("Error : returning {}, expected {} - file {}, line {} : {}\r\n".format(returnType, maskFunction.type, file, lineNum, line))
							if maskFunction.hasParam:
								paramType = getParamType(line)
								if paramType  != maskFunction.type and paramType != "":
									errors = errors + 1
									print("Error : param {}, expected {} - file {}, line {} : {}\r\n".format(paramType, maskFunction.type, file, lineNum, line))
	return errors
			
		
		
mypath = "../tracker2"
directories = listdir(mypath)
pathfiles = []
#print(directories)
for directory in directories:
	dirpath = join(mypath,directory)
	if(isdir(dirpath)):
		files = listdir(dirpath)
		#print(files)
		for file in files:
			pathfile = join(dirpath, file)
			pathfiles.append(pathfile.replace("\\","/"))
#onlyfiles = [f for f in listdir(mypath) if isfile(join(mypath, f))]
#print(pathfiles)
maskFunctionList = []
parseMaskFunctions(maskFunctionList)
#print(maskFunctionList)
errors = checkMaskFunctionUse(maskFunctionList, pathfiles)
print("{} errors found.".format(errors))