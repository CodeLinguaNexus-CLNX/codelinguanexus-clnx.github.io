import json
import os

def update_jsonl_with_c_files(jsonl_file_path, base_folder, output_jsonl_file):
    updated_jsonl_data = []

    with open(jsonl_file_path, 'r', encoding='utf-8') as file:
        for line in file:
            # Parse each line of JSON
            data = json.loads(line)
            idx = data['idx']

            # Construct the path for the corresponding .c file
            c_file_path = os.path.join(base_folder, str(idx), f"{idx}.c")

            # Read the content of the .c file and update the 'func' field
            if os.path.exists(c_file_path):
                with open(c_file_path, 'r', encoding='utf-8') as c_file:
                    data['func'] = c_file.read()
            
            updated_jsonl_data.append(data)

    # Write to the new JSONL file
    with open(output_jsonl_file, 'w', encoding='utf-8') as outfile:
        for entry in updated_jsonl_data:
            json.dump(entry, outfile)
            outfile.write('\n')

# Example usage
jsonl_file_path = 'path/to/your/original_jsonl_file.jsonl'  # Replace with the path of the original JSONL file
base_folder = 'path/to/your/base_folder'  # Replace with your base folder path
output_jsonl_file = 'path/to/your/new_jsonl_file.jsonl'  # Replace with the path of the new JSONL file

# Call the function to process the JSONL file
update_jsonl_with_c_files(jsonl_file_path, base_folder, output_jsonl_file)
