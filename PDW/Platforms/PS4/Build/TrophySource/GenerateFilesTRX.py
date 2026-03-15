from os.path import join, dirname, abspath
import codecs
import json
import re
import sys
from collections import OrderedDict
from pyexcel_ods3 import get_data
#import os

# -----------------------------------------------------------------------------------------+
# config - config - config - config - config - config - config - config - config - config  |
# -----------------------------------------------------------------------------------------+

PATH = str(sys.argv[0])

# .ods file w/ spreadsheet
ODS_FILE = 'AchievementsData.ods'

LANG_ARRAY = ['Japanese','English','French','Spanish','German','Italian','Dutch','Portuguese','Russian','Korean','Trad.Chinese','Simp.Chinese','Finnish','Swedish','Danish','Norwegian','Polish','Braz.Portuguese','UK English','Turkish','Latin American Spanish','Arabic','Canadian French','Czech','Hungarian','Greek','Romanian','Thai','Vietnamese','Indonesian']

REF_NAME = 'Name'
REF_DESCRIPTION = 'Description'

DEFAULT_LANGUAGE = 'Default Language'

# -----------------------------------------------------------------------------------------+
# config - config - config - config - config - config - config - config - config - config  |
# -----------------------------------------------------------------------------------------+

def readodsfile(filename):
	return get_data(filename)

if __name__ == "__main__":
	# DISABLING THE SCRIPT
	#sys.exit()

	#get the project name
	splitted = PATH.split('PRJ\\')
	splitted = splitted[1].split('\\')	
	projectName = splitted[0]

	sourceFile = open(projectName + '.trx', mode="r", encoding='utf-8')

	outputFile = open(projectName + '.new.trx', mode="w", encoding='utf-8')

	#copy header from source file
	for line in sourceFile:
		outputFile.write(line)
		if('</file>' in line):
			break

	odsContent = readodsfile(ODS_FILE)	

	Languages = []
	for entry in odsContent[REF_NAME][0]:
		Languages.append(entry);

	#contain trophy name and descrition to put into the xml
	lang_dictionary = {}

	i = 0;
	for currentLanguage in Languages:
	#currentLanguage = Languages[i]
		#print("Current Language= "+currentLanguage)
		header = odsContent[REF_NAME][0]

		#column id
		language_colid = -1
		for col_id, entry in enumerate(header):
			if entry == currentLanguage:
				language_colid = col_id
				#print('entry ' + str(entry) + ' language_colid = ' + str(language_colid))

		if language_colid == -1:
			print('error 1')
			sys.exit()

		trophyDataName = {}
		trophyDataDescription = {}

		j = 0
		for row in odsContent[REF_NAME]:
			row1 = odsContent[REF_DESCRIPTION][j]
			name = str(row[language_colid] if len(row) > language_colid else '')
			description = str(row1[language_colid] if len(row1) > language_colid else '')

			if bool(description and description.strip()) and bool(name and name.strip()):
				if (j!=0):
					trophyDataName[j] = name
					trophyDataDescription[j] = description
				j=j+1;

		# try:
			# tropId = LANG_ARRAY.index(currentLanguage)
			# #print(tropId)
		# except:
			# #print("---------------------------")
			# pass		

		lang = ''

		tropId = -1

		if(currentLanguage==DEFAULT_LANGUAGE):
			lang += ' <file name="TROP.SFM" path="">' + '\n'
		else:
			tropId = LANG_ARRAY.index(currentLanguage)
			lang += ' <file name="TROP_' + f'{tropId:02}' + '.SFM" path="">' + '\n'

		lang += '  <trophyconf version="1.1" platform="ps4" policy="large">' + '\n'

		#print('lang ' + currentLanguage + ' tropId = ' + str(tropId) + ' ' + str(i))

		j = 1
		for k,ne in trophyDataName.items():
			if (j==1):
				titleName = trophyDataName[j]
				titleDetail = trophyDataDescription[j]
				lang += "   <title-name>" + titleName.strip() +"</title-name>\n"
				lang += "   <title-detail>" + titleDetail.strip() +"</title-detail>\n"
				#print(titleName)
			else:
				n = trophyDataName[j]
				d = trophyDataDescription[j]
				id = j-2
				lang += "   <trophy id=\"" + f'{id:03}' +"\">\n"
				lang += "    <name>" + n.strip() +"</name>\n"
				lang += "    <detail>" + d.strip() +"</detail>\n"
				lang += "   </trophy>\n"
				#print(n + '\tropId' + d)
			j=j+1

		lang += "  </trophyconf>" + '\n'
		lang += " </file>" + '\n'

		lang_dictionary[tropId]=lang

		i=i+1

	footer = []

	#sort the dicionary
	ld = OrderedDict(sorted(lang_dictionary.items()))

	#print in file sorted trophy language
	for line in ld:
		outputFile.write(ld[line])

	#load reverse file
	for line in reversed(list(sourceFile)):
		if('</file>' in line):
			break
		footer.append(line)

	#print in file trophy images
	for line in reversed(footer):
		outputFile.write(line)

	#pause when is launched from 
	#os.system("pause")

	sys.exit()