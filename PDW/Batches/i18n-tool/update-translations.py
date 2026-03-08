from os.path import join, dirname, abspath
import codecs
import json
import re
import sys
from collections import OrderedDict
from pyexcel_ods3 import get_data

#
#                 .::: tool megapower per le traduzioni :::.
#
# sto coso lavora con un file .ods (open document spreadsheet) e si aspetta che la
# riga 0 del foglio specificato sia un header con tutti i nomi delle lingue, tipo:
# [ ENGRISH | ITAGLIANO | FRANSE' | TEDESKEN ]
# si aspetta pure un Game.archive preso da /Content/Localization/Game/XY/ che è un file
# json con la mappa "meaning" <--> "traduzione".
#
# si configurano due cose qua sotto e si lancia, lo script copia le traduzioni presenti
# sull'excel su ogni file di traduzione
#
# output: nomefile.archive con le traduzioni copiate dal foglio di calcolo
# output: errors.log con gli errori trovati durante i tentativi di traduzione
#
# nota: serve python3
# nota: pyexcel_ods3 non è incluso in python di default, quindi va installato
#       (da linea di comando: pip install pyexcel_ods3)
#


# -----------------------------------------------------------------------------------------+
# config - config - config - config - config - config - config - config - config - config  |
# -----------------------------------------------------------------------------------------+

#PATH = 'D:/PRJ/BLZ'
PATH = str(sys.argv[1])
# input file from UE4 with default keys / translations to be used as reference
REF_FILE = PATH + '/Main/Content/Localization/Game/eo/Game.archive'

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

# reference language name (must match spreadsheet language name, case insensitive)
REF_LANG_NAME = 'esperanto'

# .ods file w/ spreadsheet
TRANSLATION_FILE = 'KS2_Localisations.xlsx.ods'

# sheet name
SHEET_NAME = 'Localization'

# -----------------------------------------------------------------------------------------+
# config - config - config - config - config - config - config - config - config - config  |
# -----------------------------------------------------------------------------------------+










# read file and ensure is utf-16 encoded
def readjsonfile(filename):
	try:
		f = codecs.open(filename, encoding='utf-16', errors='strict')
		for line in f:
			pass
		f.close()
		
		f = open(filename, encoding='utf-16')
		j = json.loads(f.read(), object_pairs_hook=OrderedDict)
		f.close()
		return j

	except UnicodeDecodeError:
		return None


def readodsfile(filename):
	return get_data(filename)


def replace_translation(out_jsoncontent, key_t, translation):
	verbose=1
	subst_count = 0

	for entry in out_jsoncontent['Children']:
		if entry['Key'] == key_t:
			if entry['Translation']['Text'] != translation:
				if verbose==1:
					try:
						print('> ' + entry['Translation']['Text'] + ' != ' + translation)
					except Exception as e:
						print('cannot encode to log')
				subst_count += 1
				entry['Translation']['Text'] = translation

	if "Subnamespaces" in out_jsoncontent:
		for namespace in out_jsoncontent['Subnamespaces']:
			for entry in namespace['Children']:
				if entry['Key'] == key_t:
					if entry['Translation']['Text'] != translation:
						if verbose==1:
							try:
								print('> ' + entry['Translation']['Text'] + ' != ' + translation)
							except Exception as e:
								print('cannot encode to log')
						subst_count += 1
						entry['Translation']['Text'] = translation

	return subst_count

def fill_empty_translation(out_jsoncontent):

	subst_count = 0
	
	for master in out_jsoncontent['Children']:
		masterSource = master['Source']['Text']
		masterTranslation = master['Translation']['Text']
		
		if len(masterTranslation) > 0:
			
			for entry in out_jsoncontent['Children']:
				if entry['Source']['Text'] == masterSource and (len(entry['Translation']['Text']) == 0 or entry['Translation']['Text'] != masterTranslation):
					#print('> ' + entry['Source']['Text'] + ' fill with ' + str(masterTranslation) + '<')
					subst_count += 1
					entry['Translation']['Text'] = masterTranslation
			if "Subnamespaces" in out_jsoncontent:
				for namespace in out_jsoncontent['Subnamespaces']:
					for entry in namespace['Children']:
						if entry['Source']['Text'] == masterSource and len(entry['Translation']['Text']) == 0:
							#print('> ' + entry['Source']['Text'] + ' fill with ' + masterTranslation)
							subst_count += 1
							entry['Translation']['Text'] = masterTranslation

	if "Subnamespaces" in out_jsoncontent:
		for namespace in out_jsoncontent['Subnamespaces']:
			for master in namespace['Children']:
				masterSource = master['Source']['Text']
				masterTranslation = master['Translation']['Text']
				
				if len(masterTranslation) > 0:
					
					for entry in out_jsoncontent['Children']:
						if entry['Source']['Text'] == masterSource and (len(entry['Translation']['Text']) == 0 or entry['Translation']['Text'] != masterTranslation):
							#print('> ' + entry['Source']['Text'] + ' fill with ' + str(masterTranslation) + '<')
							subst_count += 1
							entry['Translation']['Text'] = masterTranslation
					if "Subnamespaces" in out_jsoncontent:
						for namespace in out_jsoncontent['Subnamespaces']:
							for entry in namespace['Children']:
								if entry['Source']['Text'] == masterSource and len(entry['Translation']['Text']) == 0:
									#print('> ' + entry['Source']['Text'] + ' fill with ' + masterTranslation)
									subst_count += 1
									entry['Translation']['Text'] = masterTranslation

	return subst_count
	
if __name__ == "__main__":
	# DISABLING THE SCRIPT
	#sys.exit()

	# reference (meaning - english)
	filename = REF_FILE
	ref_json_content = readjsonfile(filename)
	if not ref_json_content:
		print('could not find reference file ' + filename)
		exit

	ref_counter = 0
	reference_translation = {}
	for entry in ref_json_content['Children']:
		xls_key = entry['Translation']['Text'].lower().strip()
		#archive_key = entry['Source']['Text'].lower().strip()
		archive_key = entry['Key']
		reference_translation[xls_key] = archive_key
		ref_counter = ref_counter + 1

	if "Subnamespaces" in ref_json_content:
		for subnamespaces in ref_json_content['Subnamespaces']:
			for subentry in subnamespaces['Children']:
				xls_key = subentry['Translation']['Text'].lower().strip()
				#archive_key = subentry['Source']['Text'].lower().strip()
				archive_key = subentry['Key']
				reference_translation[xls_key] = archive_key
				ref_counter = ref_counter + 1

	print(filename + ': found ' + str(ref_counter) + ' meanings\n')


	#
	#
	#

	for lang_file, lang_name in FILE.items():
		
		error_output = open('errors_' + lang_name.replace(' ', '_') + '.log', 'w', encoding='utf-8')

		# TODO: loop through i18n folders inside UE4 content
		filename = PATH + lang_file
		json_content = readjsonfile(filename)
		if not json_content:
			print('file ' + filename + ' does not exist or contains invalid data :/')
			exit

		if not 'Children' in json_content:
			print('invalid file format (missing Children key in json structure)')


		string_count = 0
		translated_count = 0
		ue4_strings = []
		ue4_keys = []
		for entry in json_content['Children']:

			if not 'Source' in entry or not 'Translation' in entry:
				print('[EE] invalid entry found')
				pass

			if not 'Text' in entry['Source'] or not 'Text' in entry['Translation']:
				print('[EE] invalid entry found')
				pass

			translated = bool(entry['Translation']['Text'] and entry['Translation']['Text'].strip()) and entry['Key'] in reference_translation.values()
			#print('[**] \"' + entry['Source']['Text'] + '\" (translated: ' + str(translated) + ')')

			string_count = string_count + 1
			ue4_keys.append(entry['Key'])
			if translated:
				ue4_strings.append(entry['Source']['Text'].lower().strip())
				translated_count = translated_count + 1
			else:
				if not bool(entry['Translation']['Text'] and entry['Translation']['Text'].strip()):
					error_output.write('[*] \"' + entry['Source']['Text'] + '\" has no translation\n')
				elif not entry['Key'] in reference_translation.values():
					error_output.write('[*] \"' + entry['Source']['Text'] + '\" has a key (' + entry['Key'] + ') not found in reference file\n')

		if 'Subnamespaces' in json_content:
			for subnamespaces in json_content['Subnamespaces']:

				if not 'Children' in subnamespaces:
					pass

				for subentry in subnamespaces['Children']:
					if not 'Source' in subentry or not 'Translation' in subentry:
						print('[EE] invalid entry found')
						pass

					if not 'Text' in subentry['Source'] or not 'Text' in subentry['Translation']:
						print('[EE] invalid entry found')
						pass

					translated = bool(subentry['Translation']['Text'] and subentry['Translation']['Text'].strip()) and subentry['Key'] in reference_translation.values()
					string_count = string_count + 1

					ue4_keys.append(subentry['Key'])
					if translated:
						ue4_strings.append(subentry['Source']['Text'].lower().strip())
						translated_count = translated_count + 1
					else:
						if not bool(subentry['Translation']['Text'] and subentry['Translation']['Text'].strip()):
							error_output.write('[*] \"' + subentry['Source']['Text'] + '\" has no translation\n')
						elif not subentry['Key'] in reference_translation.values():
							error_output.write('[*] \"' + subentry['Source']['Text'] + '\" has a key (' + subentry['Key'] + ') not found in reference file\n')

		print('[' + lang_name + '] ' + filename + ': ' + str(translated_count) + ' out of ' + str(string_count) + ' entries translated ({0:.2f}%)'.format(100 * translated_count / string_count))

		out_jsoncontent = json_content

		#
		#
		#

		filename = TRANSLATION_FILE
		json_content = readodsfile(filename)

		if not SHEET_NAME in json_content:
			print('invalid file format (missing ' + SHEET_NAME + ' sheet)')

		header = json_content[SHEET_NAME][0]
		source_colid = -1
		trans_colid = -1
		key_colid = -1
		for col_id, entry in enumerate(header):
			if entry.lower() == REF_LANG_NAME.lower():
				source_colid = col_id
			if entry.lower() == lang_name.lower():
				trans_colid = col_id
			if entry == 'IDs':
				key_colid = col_id

		if key_colid == -1:
			print('*** WARNING *** IDs column not found, will try to match based on ' + REF_LANG_NAME + ' language column')
			
		if source_colid == -1:
			print('translation file is missing the reference column')
			exit

		if trans_colid == -1:
			print('translation file is missing the \'' + lang_name + '\' column')
			exit

		match_count = 0
		updated_ref_lang = 0
		no_match = []
		for row in json_content[SHEET_NAME]:
			source_key = str(row[key_colid] if len(row) > key_colid else '')
			source = str(row[source_colid] if len(row) > source_colid else '')
			translation = str(row[trans_colid] if len(row) > trans_colid else '')

			if bool(source and source.strip()) and bool(translation and translation.strip()):
			
				escaped_r = codecs.unicode_escape_decode('\r')[0]
				escaped_n = codecs.unicode_escape_decode('\n')[0]
				
				escaped_source = source.replace('\\n', escaped_n).replace('\\r', escaped_r)
				escaped_translation = translation.replace('\\n', escaped_n).replace('\\r', escaped_r)
			
				# update english too if we have a key
				if bool(source_key and source_key.strip()):
					updated_ref_lang += replace_translation(ref_json_content, source_key, escaped_source)					

				source_t = escaped_source.lower().strip()
				key_t = reference_translation[source_t] if source_t in reference_translation else ''
					
				found = False
				if source_t in reference_translation and key_t in ue4_keys:
					found = True
					match_count += replace_translation(out_jsoncontent, key_t, escaped_translation)

				if source_key in ue4_keys:
					found = True
					match_count += replace_translation(out_jsoncontent, source_key, escaped_translation)
				
				if not found:
					no_match.append(source_t)
					#print(source_t)     
					error_output.write('[X] Could not find \"' + source +'\"' + '\n')
					
		#fill empty translation
		empty_translation_count = fill_empty_translation(out_jsoncontent)
		#match_count += empty_translation_count
		if (empty_translation_count > 0):
			print('[' + lang_name + '] ' + filename + ': ' + str(empty_translation_count) + ' empty translation fixed')

		#save files
		print('[' + lang_name + '] ' + filename + ': ' + str(match_count) + ' strings out of ' + str(translated_count) + ' found matching translation file ({0:.2f}%)\n'.format(100 * match_count / string_count))

		better_json = re.sub(r'^((\s*)".*?":)\s*([{])', r'\1\n\2\3', json.dumps(out_jsoncontent, indent=4, ensure_ascii=False), flags=re.MULTILINE)
		
		#resfile = open(lang_file + '.new', 'w', encoding="utf-16")
		resfile = open(PATH + lang_file, 'w', encoding="utf-16")
		resfile.write(better_json)
		resfile.close()
		
		if updated_ref_lang > 0:
			better_json = re.sub(r'^((\s*)".*?":)\s*([{])', r'\1\n\2\3', json.dumps(ref_json_content, indent=4, ensure_ascii=False), flags=re.MULTILINE)
			reffile = open(REF_FILE, 'w', encoding="utf-16")
			reffile.write(better_json)
			reffile.close()
			print('ref language modified, updating')
			
