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

def translate_source_token(source_token, token_type):
    # Optimized dictionary definition
    translations = {
        # Operator symbols:
        # Pointer operators
        '&': 'bitwise AND' if before and after and not before.isspace() and not after.isspace() else 'obtain address of ',
        '*': 'multiply' if before and after and not before.isspace() and not after.isspace() else 'dereference',
        # Bitwise operators
        '|': 'bitwise OR',                 # Bitwise OR
        '^': 'bitwise XOR',                # Bitwise XOR
        '~': 'bitwise NOT',                # Bitwise NOT
        # Shift operators
        '<<': 'left shift by',                # Left shift
        '>>': 'right shift by',               # Right shift


        # API call symbols
        # Memory management
        'malloc': 'allocate memory of',    # Allocate specified size of memory
        'free': 'deallocate memory of',    # Deallocate memory
        # Thread synchronization
        'pthread_create': 'create new thread', # Create new thread
        # System calls
        'write': 'write data to file descriptor', # Write data to file descriptor


        # Control structures
        'goto': 'jump to the statement',                 # Jump to goto label
        'setjmp': 'save the current environment',     # setjmp function
        'longjmp': 'perform a non-local jump to the environment',   # longjmp function



        # Data types and declaration symbols
        # Basic data types
        'int': 'integer type or return',                        # Integer
        'char': 'character type or return',                     # Character
        'float': 'floating point type or return',               # Floating point
        'double': 'double floating point type or return',       # Double precision floating point
        'void': 'void type or return',                          # Void
        # Complex type systems
        'struct': 'declare a structure',                   # Structure
        'union': 'define a union',                        # Union
        'enum': 'define an enumeration type',                   # Enumeration
        # Classes and templates (specific to C++)
        'class': 'class definition with method',     # Define a class and its methods
        'template': 'template class definition',  # Define a template class
        # Other type modifiers
        'const': 'declare constant',            # Constant modifier
        'static': 'declare static ',             # Static modifier
        'volatile': 'declare volatile',         # Volatile modifier
        'unsigned': 'declare non-negative',         # Unsigned modifier
        'signed': 'declare negative or positive',             # Signed modifier

        # Preprocessor directive symbols 
        # Header Inclusions
        '#include': 'include header file',  # Include a header file
        # Macro Definitions
        '#define': 'define macro',    # Define a macro
        # Conditional Compilation
        '#ifdef': 'if it defined',       # If a macro is defined
        '#endif': 'end if defined',   # End of if defined block
        '#if': 'conditional compilation', # Conditional compilation based on condition
        '#else': 'else condition',    # Else condition in conditional compilation
        '#elif': 'else if condition', # Else if condition in conditional compilation
        # Other preprocessor logic can be added here...
    }

    return translations.get(source_token, source_token)


def preprocess_code(source_code):
    """
    Process the source code to replace tokens based on the provided dictionary.
    """
    tokens = re.findall(r'(\b\w+\b|->\*|->|\+\+|--|==|!=|<=|>=|\+=|-=|\*=|/=|%=|<<=|>>=|&=|\|=|\^=|\?\s*:|::|[+\-*/&|^<>=!~.,;])', source_code)
    processed_code = ""
    last_index = 0

    for token in tokens:
        start_index = source_code.find(token, last_index)
        processed_code += source_code[last_index:start_index]
        processed_code += translate_source_token(token, "token_type")
        last_index = start_index + len(token)

    processed_code += source_code[last_index:]
    return processed_code

def process_c_files(directory):
    """
    Process all .c files in the directory by replacing tokens with their translations.
    """
    c_files = find_c_files(directory)
    for file_path in c_files:
        with open(file_path, 'r') as file:
            source_code = file.read()

        processed_code = preprocess_code(source_code)

        with open(file_path, 'w') as file:
            file.write(processed_code)

# Directory to process
directory = "path/to/your/data_directory"  # Replace with your specific directory path

# Process all .c files in the directory
process_c_files(directory)
