import json
import requests
import re
import subprocess


def get_func_name(func):
    # extract fun_name
    end_index = func.find("\n")
    if end_index != -1:
        func_name = func[:end_index]
        return func_name
    else:
        return 0
        

def get_file_path(project, commit_id, func_name, session, headers):
    # construct URL to search for page.
    url = f"https://github.com/{project}/{project}/commit/{commit_id}.patch"
    response = session.get(url, headers=headers)
    patch_content = response.text
    # search for func_name line
    lines = patch_content.split("\n")
    function_line = None
    for i, line in enumerate(lines):
        if func_name in line:
            function_line = line
            function_line_number = i
            break
    
    if function_line==None: 
        for i, line in enumerate(lines):
            end_index = func_name.find("(")
            if end_index != -1:
                part_func_name = func_name[:end_index+1]
            if part_func_name in line:
                function_line = line
                function_line_number = i
                break     

            
    if function_line:
        for j in range(function_line_number - 1, -1, -1):
            if lines[j].startswith("+++ b/"):
                file_path = lines[j][5:]
                print("file_path=", file_path)
                return file_path
    else:
        return 0

def get_func_after(project, commit_id, func_name, file_path,session, headers):
    # construct file URL to get func_after
    url = f"https://raw.githubusercontent.com/{project}/{project}/{commit_id}{file_path}"
    response = session.get(url, headers=headers)
    file_content = response.text
    # extract full function from file
    position = 0
    
    while True:
        function_start = file_content.find(func_name, position)
        if function_start == -1:
            return 0
        
        end_of_line = file_content.find('\n', function_start)
        if file_content[end_of_line - 1] != ';':
            break
        else:
            position = end_of_line        
    
    function_end = file_content.find("\n}", function_start)

    if function_end != -1:
        func_after = file_content[function_start:function_end+2]
        return func_after
    else:
        return 0




# You JSONL file
with open("data.jsonl", "r") as file, open("data_add_after.jsonl","w") as new_file:
    session = requests.Session()
    headers = {
    'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.3'
    }
    for line in file:
        data = json.loads(line)
        project = data["project"]
        commit_id = data["commit_id"]
        target = data["target"]
        func = data["func"]
        idx = data["idx"]

        func_name = get_func_name(func)
        file_path = get_file_path(project, commit_id, func_name, session, headers)
        func_after = get_func_after(project, commit_id, func_name, file_path, session, headers)

        data["func_name"] = func_name
        data["file_path"] = file_path
        data["func_after"] = func_after    
        
        new_file.write(json.dumps(data))
        new_file.write("\n")

    new_file.close()
