import os
import re

def find_c_files(directory):
    """
    Recursively find all .c files in the given directory and its subdirectories.
    """
    c_files = []
    for root, dirs, files in os.walk(directory):
        for file in files:
            if file.endswith(".c"):
                c_files.append(os.path.join(root, file))
    return c_files

def preprocess_comments(source_code):
    """
    Replace single-line and multi-line comments with text content in the source code.
    """
    # Replace single-line comments
    processed_code = re.sub(r'//.*', lambda x: 'Note: ' + x.group(0)[2:] + ' End of note;', source_code)
    
    # Replace multi-line comments
    def replace_multiline_comment(match):
        comment = match.group(0)
        comment = comment.replace('/*', 'Note: ')
        comment = comment.replace('*/', ' End of note;')
        comment_lines = comment.split('\n')
        processed_comment = '; '.join(comment_lines) if len(comment_lines) > 1 else comment
        return processed_comment

    processed_code = re.sub(r'/\*[\s\S]*?\*/', replace_multiline_comment, processed_code)
    return processed_code

def process_c_files(directory):
    """
    Process all .c files in the directory by replacing comments with text content.
    """
    c_files = find_c_files(directory)
    for file_path in c_files:
        with open(file_path, 'r') as file:
            source_code = file.read()

        processed_code = preprocess_comments(source_code)

        with open(file_path, 'w') as file:
            file.write(processed_code)

# Directory to process
directory = "your/path/to/code_directory"

# Process all .c files in the directory
process_c_files(directory)
