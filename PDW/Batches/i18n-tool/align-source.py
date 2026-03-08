from os.path import join, dirname, abspath
import codecs
import json
import re
import sys
from collections import OrderedDict
from pyexcel_ods3 import get_data

#
#			   .::: tool megapower per le traduzioni (parte 2) :::.					   
#
# sto coso invece prende un file con il linguaggio di riferimento (quello scelto come nativo
# su ue4) e verifica che "Source" e "Translation" siano coerenti. In caso non lo siano,
# siccome unreal è una #?@!@ (merda) e usa come _chiave_ la Translation invece del Source
# per matchare le traduzioni, lo script allinea tutto
#
# nella sua prima fase la translation in inglese è sostituita con il contenuto della traduzione
# sull'excel (che quindi si assume come fonte dati "affidabile")
#


# -----------------------------------------------------------------------------------------+
# config - config - config - config - config - config - config - config - config - config  |
# -----------------------------------------------------------------------------------------+

                       
PATH = str(sys.argv[1])

# input file from UE4 with default keys / translations to be used as reference
REF_FILE = PATH + '/Main/Content/Localization/Game/eo/Game.archive'

# 
MAN_FILE = PATH + '/Main/Content/Localization/Game/Game.manifest'

# pair GameArchive : LanguageName (must match spreadsheet language name, case insensitive)
FILE = { 
		'/Main/Content/Localization/Game/en/Game.archive'		: 'us english', 
		'/Main/Content/Localization/Game/en-GB/Game.archive'	: 'uk english', 
		'/Main/Content/Localization/Game/fr/Game.archive'		: 'french',
		'/Main/Content/Localization/Game/it/Game.archive'		: 'italian',
		'/Main/Content/Localization/Game/de/Game.archive'		: 'german',
		'/Main/Content/Localization/Game/es/Game.archive'		: 'spanish',			  
		'/Main/Content/Localization/Game/es-419/Game.archive'	: 'spanish latam',
		'/Main/Content/Localization/Game/pt-BR/Game.archive'	: 'brazilian portuguese',
		'/Main/Content/Localization/Game/zh-Hans/Game.archive'	: 'simplified chinese',
		'/Main/Content/Localization/Game/zh-Hant/Game.archive'	: 'traditional chinese',        
		'/Main/Content/Localization/Game/ja/Game.archive'		: 'japanese',
		'/Main/Content/Localization/Game/ko/Game.archive'		: 'korean',
		'/Main/Content/Localization/Game/ar/Game.archive'		: 'arabic',
		'/Main/Content/Localization/Game/pl/Game.archive'		: 'polish',
		'/Main/Content/Localization/Game/nl/Game.archive'		: 'dutch',
		'/Main/Content/Localization/Game/sv/Game.archive'		: 'swedish',
		'/Main/Content/Localization/Game/no/Game.archive'		: 'norwegian',
		'/Main/Content/Localization/Game/da/Game.archive'		: 'danish',
		'/Main/Content/Localization/Game/fi/Game.archive'		: 'finnish',
}



# .ods file w/ spreadsheet
TRANSLATION_FILE = 'KS2_Localisations.xlsx.ods'

# sheet name
SHEET_NAME = 'Localization'

# reference language name (must match spreadsheet language name, case insensitive)
REF_LANG_NAME = 'esperanto'
# column name for keys (must match spreadsheet, case sensitive)
REF_COL_ID = 'IDs'

# -----------------------------------------------------------------------------------------+
# config - config - config - config - config - config - config - config - config - config  |
# -----------------------------------------------------------------------------------------+










# read file and ensure is utf-16 encoded
def readjsonfile(filename):
	try:
		f = codecs.open(filename, encoding='utf-16', errors='strict')
		for line in f:
			pass
		return json.loads(open(filename, encoding='utf-16').read(), object_pairs_hook=OrderedDict)

	except UnicodeDecodeError:
		return None

# output new content
def writejsonfile(filename, content):

	# pretty print and force newline after a {
	ue_json = re.sub(r'^((\s*)".*?":)\s*([{])', r'\1\n\2\3', json.dumps(content, indent=4, ensure_ascii=False), flags=re.MULTILINE)
	with open(filename, 'w', encoding="utf-16") as outfile:
		outfile.write(ue_json)


def readodsfile(filename):
	return get_data(filename)

	
#	
# DIO SANTO
#	
def get_translation_by_key(out_jsoncontent, key_t):

	for entry in out_jsoncontent['Children']:
		if entry['Key'] == key_t:
			return entry
	if "Subnamespaces" in out_jsoncontent:
		for namespace in out_jsoncontent['Subnamespaces']:
			for entry in namespace['Children']:
				if entry['Key'] == key_t:
					return entry
		
	return None

def replace_all_translations(out_jsoncontent, source, translation):
	
	replace_count = 0
	
	for entry in out_jsoncontent['Children']:
		if (entry['Translation']['Text'] == source['Translation']['Text'] or entry['Source']['Text'] == source['Source']['Text']) and entry['Translation']['Text'] != translation:
			entry['Translation']['Text'] = translation
			replace_count += 1
	if "Subnamespaces" in out_jsoncontent:
		for namespace in out_jsoncontent['Subnamespaces']:
			for entry in namespace['Children']:
				if (entry['Translation']['Text'] == source['Translation']['Text'] or entry['Source']['Text'] == source['Source']['Text']) and entry['Translation']['Text'] != translation:
					entry['Translation']['Text'] = translation
					replace_count += 1
					
	return replace_count
	
def replace_translations(out_jsoncontent, key_t, translation):

	translation_to_replace = get_translation_by_key(out_jsoncontent, key_t)
	#if translation_to_replace:
		#print('translation_to_replace:' + ' '+ key_t + ' '+ translation_to_replace['Source']['Text'] + '(*)' + translation_to_replace['Translation']['Text'])
		#print(bool(translation_to_replace['Translation']['Text'] and translation_to_replace['Translation']['Text'].strip()))
	if translation_to_replace and bool(translation_to_replace['Translation']['Text'] and translation_to_replace['Translation']['Text'].strip()):
		return replace_all_translations(out_jsoncontent, translation_to_replace, translation)
	
	return 0

	

if __name__ == "__main__":

	# DISABLING THE SCRIPT
	#sys.exit()

		#
		# substep 1 - File Opening
		#

	restructured_output = open('restructured'+ '.log', mode="w", encoding='utf-16')
	stat_output = open('stat'+ '.log', mode="w", encoding='utf-16')
	

	json_content = readodsfile(TRANSLATION_FILE)
	if not json_content or not SHEET_NAME in json_content:
		print('ods file \'' + TRANSLATION_FILE + '\' is invalid (or does not exist)')
		exit

	header = json_content[SHEET_NAME][0]
	source_colid = -1
	key_colid = -1
	for col_id, entry in enumerate(header):
		if entry.lower() == REF_LANG_NAME:
			source_colid = col_id
		if entry == REF_COL_ID:
			key_colid = col_id
	
	if source_colid == -1 or key_colid == -1:
		print('could not find Key or ' + REF_LANG_NAME + ' language column')
		exit
		
	json_game = readjsonfile(REF_FILE)
	print('searching reference file \'' + REF_FILE + '\'')
	if not json_game:
		print('could not find reference file \'' + REF_FILE + '\'')
		exit

	json_man = readjsonfile(MAN_FILE)
	print('searching man file \'' + MAN_FILE + '\'')

		#
		# substep 2 - Filling of the dictionary from the manifest & check for multiple key for the same manifest text(duplicate)
		#

	valid_double_keys = {'-WIP-','-NOT-NEEDED-'}
	man_sources = {}	
	man_entries = {}
	duplicates = {}
	double_key_in_excel={}
	double_key_in_excel_print={}
	
	duplicates_source = 0
	missing_in_manifest = 0
	double_keys = 0
	key_mismatch = 0
	source_mismatch = 0
	missing_keys = 0
	leftovers = 0
	
	for entry in json_man['Children']:
		source = entry['Source']['Text']
		key = entry['Keys'][0]['Key']
		man_entries[key] = source
		man_sources[source] = key
		mansize=len(entry['Keys'])
		if mansize > 1:
			duplicates_source += 1
			print('duplicates ' +key + ' ' + str(mansize) +' '+ source)
			for k in entry['Keys']:
				duplicates[k['Key']] = k['Key']
	if "Subnamespaces" in json_man:
		for subnamespaces in json_man['Subnamespaces']:
			for entry in subnamespaces['Children']:
				source = entry['Source']['Text']
				key = entry['Keys'][0]['Key']
				man_entries[key] = source	
				man_sources[source] = key
				mansize=len(entry['Keys'])
				if mansize > 1:
					duplicates_source += 1
					print('duplicates ' + key + ' ' + str(mansize) +' '+ source)
					for k in entry['Keys']:
						duplicates[k['Key']] = k['Key']
	
		#
		# substep 3 - Report the entry that are in the localization excel that are not in the manifest(into the game) [probably those needs to be removed]
		#

	print('-----------------------------------------------------------')				
	for row in json_content[SHEET_NAME]:
		source_key = str(row[key_colid] if len(row) > key_colid else '')
		source = str(row[source_colid] if len(row) > source_colid else '')
		if bool(source_key and source_key.strip()) and bool(source and source.strip()):
			if (source_key not in man_entries):
				isValid = 0
				if source_key != "-WIP-":
					if source_key not in duplicates:
						missing_in_manifest += 1
						print('not found in manifest: ' + source_key + ' ' + source)
					#else:
						#print('not found in manifest but is in duplicated: ' + source_key + ' ' + source)
			if source_key in double_key_in_excel:
				double_key_in_excel_print[source_key] = source_key
			double_key_in_excel[source_key]=source_key

	print('-----------------------------------------------------------')				

		#
		# substep 4 - Report the keys that are in localization excel in multiple lines
		#
	for source_key in double_key_in_excel_print:
		if source_key not in valid_double_keys:
			double_keys += 1
			print('duplicated key in excel [' + source_key + ']')

		#
		# substep 5 - Report the keys that are in localization excel but are different from the manifest (having the same source)
		#
	print('-----------------------------------------------------------')				
	for row in json_content[SHEET_NAME]:
		source_key = str(row[key_colid] if len(row) > key_colid else '')
		source = str(row[source_colid] if len(row) > source_colid else '')
		if bool(source_key and source_key.strip()) and bool(source and source.strip()):
			if source in man_sources:
				expected_key = man_sources[source] 
				if expected_key!=source_key:
					print('key mismatch manifest excel: ' + source_key + ' ' + expected_key + ' ' + source)
					key_mismatch += 1
					
	escaped_r = codecs.unicode_escape_decode('\r')[0]
	escaped_n = codecs.unicode_escape_decode('\n')[0]

	print('-----------------------------------------------------------')				
		#
		# substep 6 - Report the sources that are in localization excel source language different from the source in manifest
		#
	for row in json_content[SHEET_NAME]:
		source_key = str(row[key_colid] if len(row) > key_colid else '')
		source = str(row[source_colid] if len(row) > source_colid else '')
		outsource = source
		if bool(source_key and source_key.strip()) and bool(source and source.strip()):
			if source_key in man_entries:
				#print(found)
				sourcein = man_entries[source_key]
				if source != sourcein:
					escaped_sourcein = sourcein.replace(escaped_n,'\\n').replace(escaped_r,'\\r')		
					if escaped_sourcein != source:		
						print('different source: ' +'['+ source_key + '][' + escaped_sourcein + '][' + source +']')
						source_mismatch += 1
						outsource = sourcein
				del man_entries[source_key]
		escaped_source = outsource.replace(escaped_n,'\\n').replace(escaped_r,'\\r')		
		restructured_output.write(escaped_source + '\n')	
	print('-----------------------------------------------------------')				
		#
		# substep 7 - Report the keys that are missing in localization excel but there is a source associated
		#
	for row in json_content[SHEET_NAME]:
		source_key = str(row[key_colid] if len(row) > key_colid else '')
		source = str(row[source_colid] if len(row) > source_colid else '')
		if not bool(source_key and source_key.strip()) and bool(source and source.strip()):
			candidatekey=""
			for key, value in man_entries.items():
				if value == source:
					candidatekey=key
			print('missing key in excel: ' + '[' + candidatekey +']'+ '[' + source +']')
			missing_keys += 1

	print('-----------------------------------------------------------')				
		#
		# substep 8 - Report the leftover (all the keys that were not processed in the localization excel but are still in the manifest)
		#
	for entry in man_entries:
		print('leftover: ' +'['+ entry + '][' + man_entries[entry] + ']')
		leftovers += 1

		#
		# substep 9 - Report on the stats file 
		#
	
	stat_output.write('Duplicates: ' +str(duplicates_source) +  '\n')	
	stat_output.write('ExcelOnly: ' +str(missing_in_manifest) +  '\n')	
	stat_output.write('DoubleKeys: ' +str(double_keys) +  '\n')	
	stat_output.write('MissingKeys: ' +str(missing_keys) +  '\n')	
	stat_output.write('KeyMismatch: ' +str(key_mismatch) +  '\n')	
	stat_output.write('SourceMismatch: ' +str(source_mismatch) +  '\n')	
	stat_output.write('Leftovers: ' +str(leftovers) +  '\n')	
	            
				
				
	#sys.exit()
	
	#
	# step 1 - align translation using the source on the reference language
	#

	
	for row in json_content[SHEET_NAME]:
		source_key = str(row[key_colid] if len(row) > key_colid else '')
		source = str(row[source_colid] if len(row) > source_colid else '')
		
		if bool(source_key and source_key.strip()) and bool(source and source.strip()):
			escaped_r = codecs.unicode_escape_decode('\r')[0]
			escaped_n = codecs.unicode_escape_decode('\n')[0]
			escaped_source = source.replace('\\n', escaped_n).replace('\\r', escaped_r)
		
			cnt = 0
			cnt = replace_translations(json_game, source_key, escaped_source)
				
			if cnt > 0:
				print('key [' + source_key + ']: replaced ' + str(cnt) + ' translations with [' + source + ']' + ' [' + escaped_source + ']')
			
	writejsonfile(REF_FILE, json_game)
	
	#
	# step 2 - store translations as keys to lookup in l10n files
	#
	
	json_content = readjsonfile(REF_FILE)
	if not json_content:
		print('could not find reference file \'' + REF_FILE + '\'')
		exit

	mismatches = {}
	for entry in json_content['Children']:
		source = entry['Source']['Text']
		trans = entry['Translation']['Text']
		key = entry['Key']
		mismatches[key] = trans
	
	if "Subnamespaces" in json_content:
		for subnamespaces in json_content['Subnamespaces']:
			for entry in subnamespaces['Children']:
				source = entry['Source']['Text']
				trans = entry['Translation']['Text']
				key = entry['Key']
				mismatches[key] = trans
	
	#
	# step 3 - update source in all l10n files
	#
	
	for f, lang in FILE.items():

		json_content = readjsonfile(PATH + f)
		if not json_content:
			print('could not find reference file \'' + REF_FILE + '\'')
			exit

		fix_counter = 0
		for entry in json_content['Children']:
			source = entry['Source']['Text']
			key = entry['Key']
			if key in mismatches and source != mismatches[key]:
				# update source with new translation
				entry['Source']['Text'] = mismatches[key]
				fix_counter += 1
		if "Subnamespaces" in json_content:
			for subnamespaces in json_content['Subnamespaces']:
				for entry in subnamespaces['Children']:
					source = entry['Source']['Text']
					key = entry['Key']
					if key in mismatches and source != mismatches[key]:
						# update source with new translation
						entry['Source']['Text'] = mismatches[key]
						fix_counter += 1

		print('[' + lang + '] ' + PATH + f + ': fixed ' + str(fix_counter) + ' strings')
		writejsonfile(PATH + f, json_content)