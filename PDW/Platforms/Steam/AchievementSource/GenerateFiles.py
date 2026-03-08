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
REF_STEAM_LOCALE = 'STEAM_Locale'


# -----------------------------------------------------------------------------------------+
# config - config - config - config - config - config - config - config - config - config  |
# -----------------------------------------------------------------------------------------+



def readodsfile(filename):
	return get_data(filename)

	
def languageColumn(lang, header):
	language_colid = -1
	for col_id, entry in enumerate(header):
		if entry == lang:
			language_colid = col_id
	return language_colid

def getLocale(language):
	i=0;
	for entry in Languages:
		if(entry==language):
			return Locales[i]
		i=i+1
	return 0;
		
if __name__ == "__main__":

	# DISABLING THE SCRIPT
	#sys.exit()


	json_content = readodsfile(ODS_FILE)
	
	j=0;
	Languages = []
	Locales = []
	for entry in json_content[REF_NAME][0]:
		row = json_content[REF_STEAM_LOCALE][1]
		locale = str(row[j] if len(row) > j else '')
		Languages.append(entry);
		Locales.append(locale);
		j=j+1;


	inFile = open('..\..\..\steam_appid.txt', mode="r")
	id = inFile.read();
	print(id)
	outputFile = open(f'{id}' + '_loc_all.vdf', mode="w", encoding='utf-8')
	line = "\"lang\"";
	outputFile.write(line+ '\n')	
	line = "{";outputFile.write(line+ '\n')	

	for currentLanguage in Languages:
		print(currentLanguage)
		if len(getLocale(currentLanguage))>0:
			header = json_content[REF_NAME][0]
			language_colid = languageColumn(currentLanguage,header);
			
			if language_colid == -1:
			    print('error 1')
			    sys.exit()
			
			trophyData = {}
			
			j = 0
			for row in json_content[REF_NAME]:
			    row1 = json_content[REF_DESCRIPTION][j]
			    name = str(row[language_colid] if len(row) > language_colid else '')
			    description = str(row1[language_colid] if len(row1) > language_colid else '')
			    
			    if bool(description and description.strip()) and bool(name and name.strip()):
			        if (j>2):
			            trophyData[name] = description
			        j=j+1;
			
			line = "\t\"" + getLocale(currentLanguage) +"\"";outputFile.write(line+ '\n')	
			line = "\t{";outputFile.write(line+ '\n')	
			line = "\t\t\"Tokens\"";outputFile.write(line+ '\n')	
			line = "\t\t{";outputFile.write(line+ '\n')	
			j=0
			for n,d in trophyData.items():
			    token="\"NEW_ACHIEVEMENT_1_"+f'{j}'+"_NAME\" \""+ n +"\""
			    line = "\t\t\t" + token;outputFile.write(line+ '\n')	
			    token="\"NEW_ACHIEVEMENT_1_"+f'{j}'+"_DESC\" \""+ d +"\""
			    line = "\t\t\t" + token;outputFile.write(line+ '\n')	
			
			    j=j+1
			line = "\t\t}";outputFile.write(line+ '\n')	
			line = "\t}";outputFile.write(line+ '\n')	

	line = "}";outputFile.write(line+ '\n')	


	sys.exit()
