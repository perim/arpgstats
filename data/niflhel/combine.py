#!/usr/bin/python3

import csv
import os
import copy

# Get the directory of the script
dir_path = os.path.dirname(os.path.realpath(__file__))
main_csv_path = os.path.join(dir_path, 'main.csv')
combined_csv_path = os.path.join(dir_path, 'combined.csv')

# List to hold all the data from the CSV files
fieldnames = [ 'Type', 'Name', 'Weighting', 'Classification', 'Min', 'Max', 'Description', 'Define', 'Requires', 'Cap', 'LUID', 'Subtype' ]
data = []

# Cache of reused sections. Map of list of maps.
cache = {}

# Read main.csv to get the list of files to combine
with open(main_csv_path, 'r') as main_file:
	main_reader = csv.DictReader(main_file)
	for row in main_reader:
		file_to_read = row['File']
		file_path = os.path.join(dir_path, file_to_read)
		if os.path.exists(file_path):
			with open(file_path, 'r') as f:
				reader = csv.DictReader(f)
				for line in reader:
					dataline = {}
					for k,v in line.items():
						if k is None or k == '': continue
						assert k.strip() in fieldnames, '%s not found in accepted list of keys from %s' % (k.strip(), file_path)
						if v is None: dataline[k.strip()] = ''
						else: dataline[k.strip()] = v.strip()
					if not 'Type' in dataline or dataline['Type'] == '' or dataline['Type'] is None: continue
					if not 'Weighting' in dataline or dataline['Weighting'] == '' or dataline['Weighting'] == None:
						dataline['Weighting'] = '0' # Should fix in input data
					type = dataline['Type']
					assert 'Description' in dataline, 'No Description key in input: %s' % str(dataline)
					assert not dataline['Description'] is None, 'Invalid Description key in input: %s' % str(dataline)
					assert dataline['Description'] != '' or dataline['Weighting'][0] == '<', 'Empty Description key in input: %s' % str(dataline)
					if type[0] == '[': # cache it
						if not type in cache:
							cache[type] = []
						cache[type].append(dataline)
						continue
					if not 'Max' in dataline and 'Min' in dataline: dataline['Max'] = dataline['Min']
					if not 'Min' in dataline and 'Max' in dataline: dataline['Min'] = dataline['Max']
					assert 'Weighting' in dataline, 'No Weighting key in input: %s' % str(dataline)
					data.append(dataline)

def cache_fetch(type, line, newdata):
	lookup = line['Weighting'].replace('<', '[').replace('>', ']')
	assert lookup in cache, '%s not found in cache!' % lookup
	print('Cache key %s (matches %s) for type %s (%d entries)' % (lookup, line['Weighting'], type, len(cache[lookup])))
	for addendum in cache[lookup]:
		if addendum['Weighting'][0] == '<':
			print('\tnesting cache for %s : %s -> %s -> %s' % (type, lookup, addendum['Type'], addendum['Weighting']))
			cache_fetch(type, addendum, newdata) # recursive
		else:
			print('\t"%s" from cache line %s for type %s' % (addendum['Description'], lookup, type))
			newline = copy.deepcopy(addendum)
			newline['Type'] = type
			newdata.append(newline)

# Merge in from cache
newdata = []
for line in data:
	assert 'Weighting' in line, 'WTF'
	assert len(line['Weighting']) > 0, 'WTF'
	if line['Weighting'][0] != '<':
		assert line['Weighting'].isdigit(), '%s is not a number: %s' % (line['Weighting'], line)
		newdata.append(line)
	else:
		cache_fetch(line['Type'], line, newdata)

# Write the combined data to combined.csv
with open(combined_csv_path, 'w', newline='') as combined_file:
	writer = csv.DictWriter(combined_file, fieldnames=fieldnames)
	writer.writeheader()
	writer.writerows(newdata)
print(f"Combined CSV file created at {combined_csv_path}")
