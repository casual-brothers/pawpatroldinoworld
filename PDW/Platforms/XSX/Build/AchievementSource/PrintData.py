from os.path import join, dirname, abspath
import codecs
import json
import re
import sys
from collections import OrderedDict
from pyexcel_ods3 import get_data
import xml.etree.ElementTree as ET

# -----------------------------------------------------------------------------------------+
# config - config - config - config - config - config - config - config - config - config  |
# -----------------------------------------------------------------------------------------+

# .ods file w/ spreadsheet
XML_FILE = 'achievements2017.xml'

# -----------------------------------------------------------------------------------------+
# config - config - config - config - config - config - config - config - config - config  |
# -----------------------------------------------------------------------------------------+

def readodsfile(filename):
	return get_data(filename)

if __name__ == "__main__":
    
	outputFile = open('data.csv', mode="w", encoding='utf-8')
    
	tree = ET.parse(XML_FILE)
	root = tree.getroot()

	# one specific item's data
	for elem in root:
		outputFile.write(elem[0].text.replace("AchievementNameId_", "") + "\t" + elem[4][0].text + "\t" + elem[0].text + "\t" + elem[5].text + "\t" + elem[3].text + "\t" + elem[8].text + "\t" + elem[6].text + "\n")
    
	sys.exit()