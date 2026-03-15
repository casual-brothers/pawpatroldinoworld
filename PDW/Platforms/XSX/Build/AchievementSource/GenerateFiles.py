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
REF_XBOX = 'XBOX_Achievements'
REF_XBOX_LOCALE = 'XBOX_Locale'


# -----------------------------------------------------------------------------------------+
# config - config - config - config - config - config - config - config - config - config  |
# -----------------------------------------------------------------------------------------+



def readodsfile(filename):
	return get_data(filename)

def xmlField(field, value):
	s = "<" + field +">" + value + "</" + field +">"
	return s

def xmlFieldParam(field, param, value):
	s = "<" + field +" " + param +">" + value + "</" + field +">"
	return s
	
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
		row = json_content[REF_XBOX_LOCALE][1]
		locale = str(row[j] if len(row) > j else '')
		Languages.append(entry);
		Locales.append(locale);
		j=j+1;

	trophyDataAll = []
	i = 0;	
	for currentLanguage in Languages:
		print(currentLanguage)
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
				if (j!=0):
					trophyData[name] = description
				j=j+1;


		trophyDataAll.append(trophyData)
	i=i+1

	outputFile = open('achievements2017.xml', mode="w", encoding='utf-8')
	fileHeader = "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
	outputFile.write(fileHeader + '\n')	

	fileHeader = "<Achievements2017 xmlns=\"http://config.mgt.xboxlive.com/schema/achievements2017/1\">";
	outputFile.write(fileHeader + '\n')	

	j = 0
	for n,d in trophyData.items():
		#print(f'{j}' + " " + n)
		if (j>0 and j<len(trophyData.items()) - 1):
			row = json_content[REF_XBOX][j]
			colid = 1;
			points = str(row[colid] if len(row) > colid else '')
			colid = 2;
			name = str(row[colid] if len(row) > colid else '')
			colid = 3;
			unlocked = str(row[colid] if len(row) > colid else '')
			colid = 4;
			locked = str(row[colid] if len(row) > colid else '')
			colid = 5;
			image = str(row[colid] if len(row) > colid else '')
			colid = 6;
			hidden = str(row[colid] if len(row) > colid else '')
			if(hidden == "0"):
				hidden = "false"
			if(hidden == "1"):
				hidden = "true"

			line = "\t<Achievement>";
			outputFile.write(line + '\n')	
			line = "\t\t" + xmlField("AchievementNameId",name);outputFile.write(line + '\n')	
			line = "\t\t" + xmlField("BaseAchievement","true");outputFile.write(line + '\n')	
			line = "\t\t" + xmlField("DisplayOrder",f'{j}');outputFile.write(line + '\n')	
			line = "\t\t" + xmlField("LockedDescriptionId",locked);outputFile.write(line + '\n')	
			line = "\t\t<Rewards>";outputFile.write(line + '\n')	
			line = "\t\t\t" + xmlField("Gamerscore",points);outputFile.write(line + '\n')	
			line = "\t\t</Rewards>";outputFile.write(line + '\n')	
			line = "\t\t" + xmlField("UnlockedDescriptionId",unlocked);outputFile.write(line + '\n')	
			line = "\t\t" + xmlField("IsHidden",hidden);outputFile.write(line + '\n')
			line = "\t\t" + xmlField("AchievementId",f'{j}');outputFile.write(line + '\n')	
			line = "\t\t" + xmlField("IconImageId",image);outputFile.write(line + '\n')	
			
			line = "\t</Achievement>";
			outputFile.write(line + '\n')
		j=j+1

	fileHeader = "</Achievements2017>";
	outputFile.write(fileHeader + '\n')	
	
	outputFile = open('localization.xml', mode="w", encoding='utf-8')
	fileHeader = "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
	outputFile.write(fileHeader + '\n')	

	fileHeader = "<Localization xmlns=\"http://config.mgt.xboxlive.com/schema/localization/1\">"
	outputFile.write(fileHeader + '\n')	
	fileHeader = "\t<DevDisplayLocale locale=\"en-US\" />"
	outputFile.write(fileHeader + '\n')	

	j = 0
	for n in json_content[REF_NAME]:
		#print(f'{j}' + " " + f'{len(n)}')
		if (j>1 and j<len(json_content[REF_NAME])-1 and len(n) > 0):
			row2 = json_content[REF_DESCRIPTION][j+1]
			row1 = json_content[REF_NAME][j+1]
			row = json_content[REF_XBOX][j-1]
			colid = 2;
			name = str(row[colid] if len(row) > colid else '')
			colid = 3;
			unlocked = str(row[colid] if len(row) > colid else '')
			colid = 4;
			locked = str(row[colid] if len(row) > colid else '')
			if (name==""):
				continue
			nameAc="";
			description="";

			line = "\t<LocalizedString " +"id=\"" + name +"\">";outputFile.write(line + '\n')
			for	 language in Languages:
				colid = languageColumn(language,header);
				nameAc = str(row1[colid] if len(row1) > colid else '')
#				description = str(row2[colid] if len(row2) > colid else '')
				
				locale = getLocale(language)
				if (locale.find(',') != -1):
					chunks = locale.split(',')
					for	 c in chunks:
						line = "\t\t" + xmlFieldParam("Value","locale=\"" + c + "\"", nameAc);outputFile.write(line + '\n')	
				else:
					line = "\t\t" + xmlFieldParam("Value","locale=\"" + getLocale(language) + "\"", nameAc);outputFile.write(line + '\n')	
		
			line = "\t</LocalizedString>";outputFile.write(line + '\n')

			line = "\t<LocalizedString " +"id=\"" + unlocked +"\">";outputFile.write(line + '\n')
			for	 language in Languages:
				colid = languageColumn(language,header);
#				nameAc = str(row1[colid] if len(row1) > colid else '')
				locale = getLocale(language)
				description = str(row2[colid] if len(row2) > colid else '')
				if (locale.find(',') != -1):
					chunks = locale.split(',')
					for	 c in chunks:
						line = "\t\t" + xmlFieldParam("Value","locale=\"" + c + "\"", description);outputFile.write(line + '\n')	
				else:
					line = "\t\t" + xmlFieldParam("Value","locale=\"" + getLocale(language) + "\"", description);outputFile.write(line + '\n')	

			line = "\t</LocalizedString>";outputFile.write(line + '\n')

			line = "\t<LocalizedString " +"id=\"" + locked +"\">";outputFile.write(line + '\n')				
			for	 language in Languages:
				colid = languageColumn(language,header);
#				nameAc = str(row1[colid] if len(row1) > colid else '')
				locale = getLocale(language)
				description = str(row2[colid] if len(row2) > colid else '')
				if (locale.find(',') != -1):
					chunks = locale.split(',')
					for	 c in chunks:
						line = "\t\t" + xmlFieldParam("Value","locale=\"" + c + "\"", description);outputFile.write(line + '\n')	
				else:
					line = "\t\t" + xmlFieldParam("Value","locale=\"" + getLocale(language) + "\"", description);outputFile.write(line + '\n')	

			line = "\t</LocalizedString>";outputFile.write(line + '\n')
		j=j+1

  

	fileHeader = "</Localization>"
	outputFile.write(fileHeader + '\n')	

	sys.exit()
