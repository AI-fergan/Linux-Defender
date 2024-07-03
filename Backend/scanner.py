import yara
import os
import sys
import json

RULES_DIRECTORY = "" # Directory containing YARA rule files, need to be config.
LOG_FILE = "yara.log"

def config():
    """
    This function open the config file for reading the yara rules folder.
    """
    global RULES_DIRECTORY

    #get the rules folder from the json config file
    with open('config.json', 'r') as file:
        data = json.load(file)
        RULES_DIRECTORY = data["yara-rules"]

def get_dir_files(path, getSubFiles = False):
    """
    This function get all the files / folders in given dir path.
    """
    results = []

    #loop over all the dir objects
    for root, dirs, files in os.walk(path):
        for file in files:
            results.append(os.path.join(root, file))

        #loop also over sub files / folders?
        if not getSubFiles: 
            return results
        
    return results
    

def compile_yara_rules(rules_dir):
    """
    This function compile given yara rules folder.
    """
    rule_files = get_dir_files(rules_dir)
    
    compiled_rules = {}

    #loop over all the rules in the folder and compile them
    for rule_file in rule_files:
        try:
            compiled_rules[rule_file] = yara.compile(filepath=rule_file)
        except:
            pass

    return compiled_rules

def scan_file(compiled_rules, file_path):
    """
    This function scan given file by given file path and compiled rules list.
    """
    matches = []
    results = []

    #loop over all the rules to scan the file
    for rule_file, rule in compiled_rules.items():
        try:
            match = rule.match(file_path)

            #is rule match to the file?
            if match:
                matches.append((rule_file, match))
        except:
            pass

    #add the rule to matches rules in file list
    if matches:
        for rule_file, match in matches:
            results.append(match[0].rule) 
    
    return results

def scan(path, scanType = False, scanSubFiles = False):
    """
    This function scan given path of file / folder.
    """
    results = {}
    compiled_rules = compile_yara_rules(RULES_DIRECTORY)

    #check if the path is folder or file
    if scanType: #folders
        files = get_dir_files(path, scanSubFiles)

        #loop over all the files in folder
        for file in files:
            results[file] = scan_file(compiled_rules, file)
    else:
        #scan single file
        if compiled_rules:
            results[path] = scan_file(compiled_rules, path)

    return results

def main():
    #config the project yara rules folder
    config()
    results = {}
    params = sys.argv
    
    #parser the tool python line
    if len(params) < 3:
        print("Usage: (-f / -d for file or directory) (-a if diractory it will scan also sub directorys files)")
        exit(1)
    #file
    if params[1] == "-f":
        results = scan(params[2])
    #diractory
    elif params[1] == "-d":
        scanSubFiles = False
        if len(params) == 4:
            #sub files in diractory
            scanSubFiles = params[3] == "-a"
        results = scan(params[2], True, scanSubFiles)

    #loop over log data
    with open(LOG_FILE, "w") as file:
        for file_path, rules in results.items():
            file.write(file_path + '@' + '-'.join(rules) + "\n")

if __name__ == "__main__":
    main()
