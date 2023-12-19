import pickle
import re
from cpgqls_client import CPGQLSClient, import_code_query
import os
import logging
import traceback




cpg_path = ''  
func_name = ''  
my_graph = None  


class BasicBlock:
    counter = 1 
    basic_block_dict = {} 

    def __init__(self, start_line, end_line=None):
        self.start_line = start_line
        self.end_line = end_line
        self.id = f"BB{BasicBlock.counter}"
        self.type = 'normal'
        BasicBlock.counter += 1
        # Add new node to basic_blocks_dict
        BasicBlock.basic_block_dict[self.id] = self

    def output(self):
        print("start_line=", self.start_line)
        print("end_line=", self.end_line)
        print("id=", self.id)
        print("type=", self.type)

class Graph:
    def __init__(self):
        self.graph = {}

    def add_node(self, node):
        if node not in self.graph:
            self.graph[node] = []

    def add_edge(self, from_node, to_node, weight):
        self.add_node(from_node)
        self.add_node(to_node)
        self.graph[from_node].append((to_node, weight))

    def get_connections(self, node):
        return self.graph.get(node, []) 

def extract_nodes(original_string):
    nodes = []
    current_node = None
    current_content = []

    for line in original_string.split('\n'):
        line = line.strip()
        if line.endswith('('):
            if current_node is not None:
                nodes.append((current_node, '\n'.join(current_content)))
            current_node = line[:-1].strip()
            current_content = []
        elif line == ')' and current_node is not None:
            nodes.append((current_node, '\n'.join(current_content)))
            current_node = None
            current_content = []
        elif current_node is not None:
            current_content.append(line)

    return nodes

def parse_node_content(node_type, content):
    # parse content
    id_match = re.search(r'id = (.*?),', content)
    code_match = re.search(r'code = "(.*?)",', content)
    column_number_match = re.search(r'lineNumber = (.*?)(,|$)', content)
    ControlStructureType_match = re.search(r'controlStructureType = "(.*?)",', content)

    node_dict = {
        'Node type': node_type,
        'ID': id_match.group(1) if id_match else None,
        'Code': code_match.group(1) if code_match else None,
        'Line number': column_number_match.group(1) if column_number_match else None,
        'ControlStructureType': ControlStructureType_match.group(1) if ControlStructureType_match else None
    }
    return node_dict


def execute_joern_command(command):

    result = client.execute(command)
    if result['success']:
        output = result['stdout']
    else:
        print("ERROR, Joern execute ERROR")
        return []  

    extracted_nodes = extract_nodes(output) 
    extracted_nodes = extracted_nodes[1:] 

    nodes = []
    for node_type, content in extracted_nodes:
        if node_type == 'Unknown':
            continue 
        nodes.append(parse_node_content(node_type, content))

    return nodes


def is_control_structure(node):
    return node["Node type"] == 'ControlStructure'

def get_node_number(node):
    if node["Line number"] != "None":
        Line_number_phrase = node["Line number"]
        match = re.search(r'Some\(\s*value\s*=\s*(.*?)\)', Line_number_phrase)
        if match:
            line_number = match.group(1)
            return int(line_number)
        else:
            print("ERROR1") 
            print('node["Line number"]', node["Line number"])
    else:
        node_id = node["ID"]
        command = f'cpg.method("{func_name}").ast.id({node_id}).astChildren.l'
        output = execute_joern_command(command)
        if len(output) > 0:
            child = output[0]
            return get_node_number(child)
        else:
            print("ERROR2") 

def DeepSearch(node):
    node_id = node['ID']
    command = f'cpg.method("{func_name}").ast.id({node_id}).ast.l' #Here, we are generating all AST child nodes directly, rather than just one layer.
    ast_Children = execute_joern_command(command)
    # Get the line number of the last child node.
    if len(ast_Children) > 0:
        max_line_number_node = ast_Children[-1]
        return max_line_number_node, get_node_number(max_line_number_node)+1 
    else:
        print("ERROR3")


def ControlStructure_Analyse(control_structure_node):


    ControlStructure_id = control_structure_node['ID']

    command = f'cpg.method("{func_name}").ast.id({ControlStructure_id}).astChildren.l'
    ast_Children = execute_joern_command(command)

    entry_line = get_node_number(control_structure_node)
    entry = BasicBlock(entry_line, entry_line) 

    last_node_number = DeepSearch(control_structure_node)
    exit = BasicBlock(last_node_number, last_node_number)

    # Find the type
    if control_structure_node["ControlStructureType"] == 'IF': # IF IF-ELSE IF-ELSE IF-ELSE
        Flag = 0
        Empty_Block_Flag = 0
        for child in ast_Children:
            if child['Node type'] == 'Block':
                try:
                    child_entry, child_exit = Block_Analyse(child)
                    if child_entry == None and child_exit == None:
                        Empty_Block_Flag = 1
                    else:
                        my_graph.add_edge(entry.id, child_entry.id, 1)
                        my_graph.add_edge(child_exit.id, exit.id, 1)
                except Exception as e:
                    traceback.print_exc()
                    print("child", child)
            if child['Node type'] == 'ControlStructure':

                child_ID = child['ID']
                command = f'cpg.method("{func_name}").ast.id({child_ID}).astChildren.l'
                block_Children = execute_joern_command(command)

                try:
                    child_entry, child_exit = Block_Analyse(block_Children[0])
                    if child_entry == None and child_exit == None:
                        Empty_Block_Flag = 1
                    else:
                        my_graph.add_edge(entry.id, child_entry.id, 1)
                        my_graph.add_edge(child_exit.id, exit.id, 1)
                        Flag = 1
                except Exception as e:
                    print("ERROR: child_entry, child_exit = Block_Analyse(block_Children[0])")
                    traceback.print_exc()
        if Flag==0 or Empty_Block_Flag==1:
            # If there is no 'else' structure, then you need to connect an edge from the entrance to the exit.
            my_graph.add_edge(entry.id, exit.id, 1)
            
    elif control_structure_node["ControlStructureType"] == 'WHILE':
        for child in ast_Children:
            if child['Node type'] == 'Block':
                child_entry, child_exit = Block_Analyse(child)
                if child_entry == None and child_exit == None:
                    print("There is a situation where the Block is empty within the WHIHLE loop.")
                else:
                    my_graph.add_edge(entry.id, child_entry.id, 1)
                    my_graph.add_edge(child_exit.id, exit.id, 1)
        my_graph.add_edge(entry.id, exit.id, 1)

        
    elif control_structure_node['ControlStructureType'] == 'FOR':
        count = 0
        for child in ast_Children:
            if child['Node type'] == 'Block':
                if count==1: 
                    child_entry, child_exit = Block_Analyse(child)
                    if child_entry == None and child_exit == None:
                        print("There is a situation where the Block is empty within the FOR loop.")
                    else:
                        my_graph.add_edge(entry.id, child_entry.id, 1)
                        my_graph.add_edge(child_exit.id, exit.id, 1)    
                count += 1
        my_graph.add_edge(entry.id, exit.id, 1)

    elif control_structure_node['ControlStructureType'] == 'SWITCH':
        for child in ast_Children:
            if child['Node type'] == 'Block':
                child_id = child['ID']
                command = f'cpg.method("{func_name}").ast.id({child_id}).astChildren.l'
                block_ast_Children = execute_joern_command(command)

                current_bb = None
                BasicBlock_List = []
                for block_child in block_ast_Children:
                    node_line_number = get_node_number(block_child)
                    if block_child['Node type'] == 'JumpTarget':
                        if current_bb == None:
                            current_bb = BasicBlock(node_line_number, node_line_number)
                            BasicBlock_List.append(current_bb) 
                        else:
                            current_bb.end_line = node_line_number - 1
                            current_bb = None
                            
                            current_bb = BasicBlock(node_line_number, node_line_number)
                            BasicBlock_List.append(current_bb)
                    else: 
                        current_bb.end_line = node_line_number

                for block in BasicBlock_List: 
                    my_graph.add_edge(entry.id, block.id, 1)
                    my_graph.add_edge(block.id, exit.id, 1)
                   
    else: # This is for fault tolerance
        current_bb = BasicBlock(entry_line, last_node_number-1)
        my_graph.add_edge(entry.id, current_bb.id, 1)
        my_graph.add_edge(current_bb.id, exit.id, 1)

    return entry, exit
    
# Process Block
def Block_Analyse(block_node): 
    current_bb = None
    BasicBlock_List = []
    
    entry = None
    exit = None 

    # First, you need to generate all of its child nodes for the block, and you can pass the block by its ID.
    block_node_id = block_node['ID']
    command = f'cpg.method("{func_name}").ast.id({block_node_id}).astChildren.l'
    nodes = execute_joern_command(command)

    if len(nodes)==0:
        return entry, exit
    else:
        for i, node in enumerate(nodes): # First pass, used to generate all BasicBlock nodes for this layer and store them.
            if not is_control_structure(node):
                if current_bb is None:
                    basicblock_linenumber_cc = get_node_number(node)
                    current_bb = BasicBlock(basicblock_linenumber_cc, basicblock_linenumber_cc) 
                    BasicBlock_List.append(current_bb) 
                else:
                    current_bb.end_line = get_node_number(node)
            else:
                if current_bb is not None:
                    current_bb.end_line = get_node_number(node) - 1
                    current_bb = None
        
                cs_entry, cs_exit = ControlStructure_Analyse(node) #
                cs_entry.type = 'entry'
                cs_exit.type = 'exit'
                BasicBlock_List.append(cs_entry) 
                BasicBlock_List.append(cs_exit)

    Max_len = len(BasicBlock_List)

    count = 0  
    for i, bb_node in enumerate(BasicBlock_List): # Second pass, used to connect all BasicBlocks.
        count += 1
        my_graph.add_node(bb_node.id)

        if i + 1 < Max_len:
            next_node = BasicBlock_List[i+1]
            if next_node.type == 'normal' or next_node.type == 'entry': 
                my_graph.add_edge(bb_node.id, next_node.id, 1)
        else:
            next_node = None

    try: 
        entry = BasicBlock_List[0]
        exit = BasicBlock_List[-1]

        return entry, exit
    
    except Exception as e:
        print("ERROR: BasicBlock_List[0] is NULL")
        print("nodes:", nodes)    


def analyze_func_before(subfolder_path, client_input):

    global cpg_path
    global func_name
    global my_graph 
    global joern_process
    global client
    
    client = client_input

    my_graph = Graph()

    cpg_path = os.path.join(subfolder_path, "cpg.bin")

    # import cpg_path
    import_cpg_command = f'importCpg("{cpg_path}")'
    client.execute(import_cpg_command)

    # Find func_name
    find_name_command = 'cpg.method.name.toList'
    result = client.execute(find_name_command)
    extracted_items = re.findall(r'\"(.*?)\"', result['stdout'])
    func_name = extracted_items[1]


    block_command = f'cpg.method("{func_name}").block.l'
    block_output = execute_joern_command(block_command)
    block = block_output[0]



    # start building graph
    entry, exit = Block_Analyse(block)
    if entry==None and exit==None:
        print("---The first Block without nodes---") 
        print(block)
        identify_null_file_path = os.path.join(subfolder_path, "null.c")
        with open(identify_null_file_path, "wb") as file:
            pickle.dump("null", file)


    graph_file_path = os.path.join(subfolder_path, "Graph.pkl")
    with open(graph_file_path, "wb") as file:
        pickle.dump(my_graph, file)

    basic_block_dict_path = os.path.join(subfolder_path, "basic_block_dict.pkl")
    with open(basic_block_dict_path, "wb") as file:
        pickle.dump(BasicBlock.basic_block_dict, file)

    return my_graph, BasicBlock.basic_block_dict, entry, exit



