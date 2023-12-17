import json

def split_jsonl_file(jsonl_file_path, train_path, test_path, valid_path, train_ratio=0.8, test_ratio=0.1):
    # Read the original JSONL file
    with open(jsonl_file_path, 'r', encoding='utf-8') as file:
        lines = file.readlines()

    # Calculate the size of the training, testing, and validation sets
    total_lines = len(lines)
    train_size = int(total_lines * train_ratio)
    test_size = int(total_lines * test_ratio)
    valid_size = total_lines - train_size - test_size

    # Split the data
    train_data = lines[:train_size]
    test_data = lines[train_size:train_size + test_size]
    valid_data = lines[-valid_size:]

    # Write to the training, testing, and validation set files
    for data, path in zip([train_data, test_data, valid_data], [train_path, test_path, valid_path]):
        with open(path, 'w', encoding='utf-8') as outfile:
            for line in data:
                outfile.write(line)

# Example usage
jsonl_file_path = '/home/qzq/mapping/data/demo/new_CLNX.jsonl'  # Replace with the path of the original JSONL file
train_path = '/home/qzq/mapping/data/demo/train_CLNX.jsonl'  # Replace with the path of the training set file
test_path = '/home/qzq/mapping/data/demo/test_CLNX.jsonl'  # Replace with the path of the testing set file
valid_path = '/home/qzq/mapping/data/demo/valid_CLNX.jsonl'  # Replace with the path of the validation set file

# Call the function to split the JSONL file
split_jsonl_file(jsonl_file_path, train_path, test_path, valid_path)
