from os.path import join, dirname, abspath
import codecs
import json
import re
from collections import OrderedDict
from pyexcel_ods3 import get_data, save_data

#
#               .::: tool megapower per le traduzioni (parte 3) :::.                       
#
# speriamo non serva mai più.
# partendo dalle traduzioni in inglese si genera la colonna di KEYs su un excel
#

# reference language name (must match spreadsheet language name, case insensitive)
REF_LANG_NAME = 'esperanto'

# input file from UE4 with default keys / translations to be used as reference
REF_FILE = 'D:/PRJ/KOS/Main/Content/Localization/Game/eo/Game.archive'

# .ods file w/ spreadsheet
TRANSLATION_FILE = 'KOS Localisations.xlsx.ods'

# sheet name
SHEET_NAME = 'Localisation'

# read file and ensure is utf-16 encoded
def readjsonfile(filename):
	return json.loads(open(filename, encoding='utf-16').read())


def readodsfile(filename):
	return get_data(filename)


def findkey(text, jsoncontent):

	for entry in jsoncontent['Children']:
		if text == entry['Translation']['Text']:
			return entry['Key']
		
	for subnamespaces in jsoncontent['Subnamespaces']:
		for entry in subnamespaces['Children']:
			if text == entry['Translation']['Text']:
				return entry['Key']

	return None

#
#
#

if __name__ == "__main__":

	game_content = readjsonfile(REF_FILE)
	if not game_content:
		print('could not find reference file ' + filename)
		exit

	json_content = readodsfile(TRANSLATION_FILE)
	if not SHEET_NAME in json_content:
		print('invalid file format (missing ' + SHEET_NAME + ' sheet)')

	header = json_content[SHEET_NAME][0]
	source_colid = -1
	for col_id, entry in enumerate(header):
		if entry.lower() == REF_LANG_NAME:
			source_colid = col_id

	if source_colid == -1:
		print('translation file is missing the reference column or \'' + lang_name + '\' column')
		exit

	tot_entries = 0
	found_keys = 0
	for row in json_content[SHEET_NAME]:
		source = str(row[source_colid] if len(row) > source_colid else '')
		#print('Row ' + source)

		if bool(source and source.strip()):

			tot_entries += 1

			source_t = source.strip()
			key_t = findkey(source_t, game_content)

			if bool(key_t and key_t.strip()):
				found_keys += 1
				row[0] = key_t
			else:
				print('Row ' + source)

	save_data(TRANSLATION_FILE.replace('.ods', '.keys.ods'), json_content)

	print('>> ' + str(found_keys) + '/' + str(tot_entries) + ' keys found and associated!')
