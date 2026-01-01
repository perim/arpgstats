#!/usr/bin/python3

import csv
import os

# Get the directory of the script
dir_path = os.path.dirname(os.path.realpath(__file__))
main_csv_path = os.path.join(dir_path, 'main.csv')
combined_csv_path = os.path.join(dir_path, 'combined.csv')

# List to hold all the data from the CSV files
fieldnames = [ 'Type', 'Name', 'Weighting', 'Subtype', 'Min', 'Max', 'Description', 'Define', 'Requires' ]
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
						if k == None or k == '': continue
						assert k.strip() in fieldnames, '%s not found in accepted list of keys from %s' % (k.strip(), file_path)
						dataline[k.strip()] = v.strip()
					if not 'Type' in dataline or dataline['Type'] == '' or dataline['Type'] == None: continue
					if not 'Weighting' in dataline or dataline['Weighting'] == '' or dataline['Weighting'] == None:
						dataline['Weighting'] = '0' # Should fix in input data
					type = dataline['Type']
					if type[0] == '[': # cache it
						if not type in cache:
							cache[type] = []
						cache[type].append(dataline)
						continue
					if not 'Max' in dataline and 'Min' in dataline: dataline['Max'] = dataline['Min']
					if not 'Min' in dataline and 'Max' in dataline: dataline['Min'] = dataline['Max']
					assert 'Weighting' in dataline, 'No Weighting key in input'
					assert 'Type' in dataline, 'No Type key in input'
					data.append(dataline)

# Merge in from cache
newdata = []
for line in data:
	assert 'Weighting' in line, 'WTF'
	assert len(line['Weighting']) > 0, 'WTF'
	if line['Weighting'][0] != '<':
		newdata.append(line)
	else:
		lookup = line['Weighting'].replace('<', '[').replace('>', ']')
		assert lookup in cache, '%s not found in cache!' % lookup
		print('Found cache key %s for type %s (%d entries)' % (lookup, line['Type'], len(cache[lookup])))
		for addendum in cache[lookup]:
			print('Found %s cache line %s for type %s (%d entries)' % (addendum['Description'], lookup, line['Type'], len(addendum)))
			newline = addendum
			newline['Type'] = line['Type']
			newdata.append(newline)

# Write the combined data to combined.csv
with open(combined_csv_path, 'w', newline='') as combined_file:
	writer = csv.DictWriter(combined_file, fieldnames=fieldnames)
	writer.writeheader()
	writer.writerows(newdata)
print(f"Combined CSV file created at {combined_csv_path}")
