from os.path import join, dirname, abspath
import codecs
import json
import re
import sys
from collections import OrderedDict
from pyexcel_ods3 import get_data


# -----------------------------------------------------------------------------------------+
# config - config - config - config - config - config - config - config - config - config  |
# -----------------------------------------------------------------------------------------+
			
# .ods file w/ spreadsheet
ODS_FILE = 'AchievementsData.ods'

REF_NAME = 'Name'
REF_DESCRIPTION = 'Description'

# -----------------------------------------------------------------------------------------+
# config - config - config - config - config - config - config - config - config - config  |
# -----------------------------------------------------------------------------------------+



def readodsfile(filename):
	return get_data(filename)
	

if __name__ == "__main__":

	# DISABLING THE SCRIPT
	#sys.exit()


	json_content = readodsfile(ODS_FILE)
	

	Languages = []
	for entry in json_content[REF_NAME][0]:
		Languages.append(entry);

	i = 0;	
	for currentLanguage in Languages:
	#currentLanguage = Languages[i]
		print(currentLanguage)
		header = json_content[REF_NAME][0]
		language_colid = -1
		for col_id, entry in enumerate(header):
			if entry == currentLanguage:
				language_colid = col_id
		
		if language_colid == -1:
			print('error 1')
			sys.exit()
		
		trophyDataName = {}
		trophyDataDescription = {}

		j = 0
		for row in json_content[REF_NAME]:
			row1 = json_content[REF_DESCRIPTION][j]
			name = str(row[language_colid] if len(row) > language_colid else '')
			description = str(row1[language_colid] if len(row1) > language_colid else '')
			
			if bool(description and description.strip()) and bool(name and name.strip()):
				if (j!=0):
					trophyDataName[j] = name
					trophyDataDescription[j] = description
				j=j+1;


		outputFile = open(currentLanguage + '.sfm', mode="w", encoding='utf-8')
		fileHeader = "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
		outputFile.write(fileHeader + '\n')	
		fileHeader = "<trophyconf version=\"1.1\" platform=\"ps4\" policy=\"large\">"
		outputFile.write(fileHeader + '\n')	


		j = 1
		for k,ne in trophyDataName.items():
			if (j==1):
				titleName = trophyDataName[j].strip()
				titleDetail = trophyDataDescription[j].strip()
				line = " <title-name>" + titleName +"</title-name>"
				outputFile.write(line + '\n')	
				line = " <title-detail>" + titleDetail +"</title-detail>"
				outputFile.write(line + '\n')	
			else:
				n = trophyDataName[j].strip()
				d = trophyDataDescription[j].strip()
				id = j-2
				line = " <trophy id=\"" + f'{id:03}' +"\">"
				outputFile.write(line + '\n')	
				line = "  <name>" + n  +"</name>"
				outputFile.write(line + '\n')	
				line = "  <detail>" + d  +"</detail>"
				outputFile.write(line + '\n')	
				line = " </trophy>"
				outputFile.write(line + '\n')	
				#print(n + '\t' + d)
			j=j+1

		line = "</trophyconf>"
		outputFile.write(line + '\n')	
	i=i+1
	
	sys.exit()
