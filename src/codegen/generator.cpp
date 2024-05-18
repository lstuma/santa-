#include "utils.h"
#include "generator.h"

Generator::Generator(ASTNode ast) {
	this->ast = ast;
}

bool Generator::generate() {
	/*
		Generates executable ELF file from AST and writes it to specified path. (◕‿◕✿)
		tldr: I really ******* hope this works ⊂◉‿◉つ

		rtype: whether generation was successful
	*/

	std::cout << Color::bhyellow << "Testing Code Generator Functions\n" << Color::reset;
	std::cout << Color::bhblue + "1. " + Color::reset + "Add DataEntry(testvar, 0x08048000)\n";
	this->data_table.add("testvar", 0x08048000, 5);
	std::cout << this->data_table.to_string();
	std::cout << "found: " << this->data_table.find("testvar")->to_string() << "\n";
	std::cout << Color::bhblue + "2. " + Color::reset + "Add FunctionEntry(testfunc, 5, 0x08048000)\n";
	this->function_table.add("testfunc", 5, 0x08048000, ASTNode());
	std::cout << this->function_table.to_string();
	std::cout << "found: " << this->function_table.find("testfunc", 5)->to_string() << "\n";
	
	// 32 bit (ELFCLASS32), little endian (ELFDATA2LSB)
	this->writer.create(ELFIO::ELFCLASS32, ELFIO::ELFDATA2LSB);

	// linux abi (ELFOSABI_LINUX)
	this->writer.set_os_abi(ELFIO::ELFOSABI_LINUX);
	// executable file (ET_EXEC)	
	this->writer.set_type(ELFIO::ET_EXEC);
	// intel 80386 processor architecture
	this->writer.set_machine(ELFIO::EM_386);

	/*
		Create text section
	*/
	ELFIO::section* text_sec = writer.sections.add(".text");
	// section type is program code/data (SHT_PROGBITS)
	text_sec->set_type(ELFIO::SHT_PROGBITS);
	// load section into memory upon execution + readable (SHF_ALLOC), section is executable (SHF_EXECINSTR) 
	text_sec->set_flags(ELFIO::SHF_ALLOC | ELFIO::SHF_EXECINSTR);
	// align section by multiple of 16
	text_sec->set_addr_align(0x10);

	/*
		Create data section
	*/
	ELFIO::section* data_sec = writer.sections.add(".data");
	// section type is program code/data (SHT_PROGBITS)
	data_sec->set_type(ELFIO::SHT_PROGBITS);
	// load section into memory upon execution + readable (SHF_ALLOC), section is writable (SHF_WRITE)
	data_sec->set_flags(ELFIO::SHF_ALLOC | ELFIO::SHF_WRITE);
	// align section by multiple of 4
	data_sec->set_addr_align(0x0);

	// create needed segments
	ELFIO::segment* text_seg = writer.segments.add();
	// load segment into memory upon execution (PT_LOAD)
	text_seg->set_type(ELFIO::PT_LOAD);
	ELFIO::segment* data_seg = writer.segments.add();
	// load segment into memory upon execution (PT_LOAD)
	data_seg->set_type(ELFIO::PT_LOAD);
	
	/*
		Generate text and data sections from AST
	*/
	bool generation_success = this->generate_segments(text_sec, data_sec, text_seg, data_seg);
	if(!generation_success) return false;

	/*
		Text segment
	*/
	// readable and executable
	text_seg->set_flags(ELFIO::PF_X | ELFIO::PF_R);
	text_seg->set_align(0x1000);

	/*
		Data segment
	*/
	// readable and writable
	data_seg->set_flags(ELFIO::PF_R | ELFIO::PF_W);
	data_seg->set_align(0x0);


	data_seg->add_section_index( data_sec->get_index(), data_sec->get_addr_align() );
	text_seg->add_section_index( text_sec->get_index(), text_sec->get_addr_align() );

	writer.set_entry(0x08048000);
	writer.save("hello_i386_32");

	return true;
}

bool Generator::generate_segments(ELFIO::section* text_sec, ELFIO::section* data_sec, ELFIO::segment* text_seg, ELFIO::segment* data_seg) {
	/*
		Generates text and data sections of ELF program.
		1. generate data section (pretty uncomplicated)
		2. generate text section (ಥ﹏ಥ) -> this part is worse than torture

		text_sec: pointer to .text section
		data_sec: pointer to .data section
		text_seg: pointer to .text segment
		data_seg: pointer to .data segment
	*/

	/*	
	Debugging ("Hello World")
	char data[] = { '\x48', '\x65', '\x6C', '\x6C', '\x6F', // “Hello, World!\n”
		'\x2C', '\x20', '\x57', '\x6F', '\x72',
		'\x6C', '\x64', '\x21', '\x21', '\x0A' };
	data_sec->set_data(data, sizeof(data));*/


	char text[] = { '\xB8', '\x04', '\x00', '\x00', '\x00', 	// mov eax, 4
		'\xBB', '\x01', '\x00', '\x00', '\x00', 				// mov ebx, 1
		'\xB9', '\x23', '\x80', '\x04', '\x08', 				// mov ecx, msg
		'\xBA', '\x11', '\x00', '\x00', '\x00', 				// mov edx, 17
		'\xCD', '\x80', 										// int 0x80
		'\xB8', '\x01', '\x00', '\x00', '\x00', 				// mov eax, 1
		'\xCD', '\x80' }; 										// int 0x80
	text_sec->set_data(text, sizeof(text));
	
	std::cout << Color::bhyellow << "generating .data segment...\n" << Color::reset;
	this->generate_data(data_sec);
	std::cout << Color::bhyellow << "finished generating .data segment...\n" << Color::reset;
	std::cout << this->data_table.to_string();

	std::cout << Color::bhyellow << "generating .data segment...\n" << Color::reset;
	this->generate_text(text_sec);
	std::cout << Color::bhyellow << "finished generating .data segment...\n" << Color::reset;
	std::cout << this->function_table.to_string();

	data_seg->set_virtual_address(0x08048000 + sizeof(text));
	// the physical address is not really needed for our purpose since we are only creating statically linked executable binaries
	data_seg->set_physical_address(0x08048000 + sizeof(text));

	text_seg->set_virtual_address(0x08048000);
	text_seg->set_physical_address(0x08048000);


	return true;
}

void Generator::generate_data(ELFIO::section* data) {
	/*
		goes over entire abstract syntax tree and collects all global variables, then:
		- puts them all as DataEntry Objects into DataTable
		- generates a string representing data section that contains all the variables and fills .data section

		data: pointer to .data section
		rtype: 
	*/
	uint32_t current_pos = 0;

	for(ASTNode node: this->ast.children)
		if(node.type == ast_variable_definition) {
			DataEntry* d = this->data_table.find(node[0].token.raw); 
			if(d) {
				// variable has been defined before, update value
				d->value = this->eval_static_expression(node[1]);
			} else {
				// new variable definition
				int value = this->eval_static_expression(node[1]);
				this->data_table.add(node[0].token.raw, (current_pos++)*4, value);
			}
		}

	int data_content_size = this->data_table.table.size()*4;
	char data_content_c[data_content_size];
	
	int i = 0;
	for(DataEntry entry: this->data_table.table) {
		data_content_c[i++] = (char) entry.value;
		for(int o = 0; o < 3; o++) data_content_c[i++] = 0;
	}

	data->set_data(data_content_c, data_content_size+1);

	std::cout << Color::bhyellow + ".data segment:" + Color::reset + "(size=" + Color::bhyellow + std::to_string(data_content_size) + Color::reset + ")";
	std::cout << utils::char_arr_to_hex(data_content_c, data_content_size) << "\n";
}

int Generator::eval_static_expression(ASTNode expr) {
	/*
		evaluates an expression made up of only global variable which already have a value
	*/
	if(expr[0].type == ast_value) return expr[0].token.num;
	if(expr[0].type == ast_identifier) return this->data_table.find(expr[0].token.raw)->value;

	else {
		int expr1 = eval_static_expression(expr[1]);
		int expr2 = eval_static_expression(expr[2]);
		switch((char) expr[0].token.raw.c_str()[0]) {
			case '+':
				return expr1 + expr2;
			case '-':
				return expr1 - expr2;
			case '*':
				return expr1 * expr2;
			case '/':
				return expr1 / expr2;
			case '%':
				return expr1 % expr2;
		}
	}
	// should not happen
	return 0;
}

void Generator::generate_func_table(ASTNode node) {
	/*
		- fills function table with functions (apart from address, which will be added after code generation)
		
		node: parent node which's children will be checked for function definitions
		rtype:
	*/
	for(ASTNode child: node.children) {
		if(child.type == ast_func_definition) {
			std::string identifier = child[0].token.raw;
			int argc = child[1].childcount();
			// address will be added after funtion has been generated
			uint32_t address = 0x00000000;
			this->function_table.add(identifier, argc, address, child);
		} else {
			generate_func_table(child);
		}
	}
}

void Generator::generate_text(ELFIO::section* text) {
	/*
		- generates the .text segment

		text: pointer to the ELFIO::section text section object
		rtype: 
	*/

	// find all functions
	this->generate_func_table(this->ast);

	// length of .text segment
	int length;

	for(ASTNode node: this->ast.children) {
		switch(node.type) {
			case ast_func_definition:
				break;
			case ast_func_call:
				break;
			case ast_conditional:
				break;
		}
	}
}

std::string Generator::generate_text_var_def(ASTNode node, int* length) {
	/*
		- generates machine code for a variable definition to be placed in .text segment

		node: ast node corresponding to the variable definition
		length: pointer to length variable for code length, will be updated after code has been generated
		rtype: generated code
	*/

	return "";
}

std::string Generator::generate_text_func_def(ASTNode node, int* length) {
	/*
		- generates machine code for a function definition to be placed in .text segment

		node: ast node corresponding to the function definition
		length: pointer to length variable for code length, will be updated after code has been generated
		rtype: generated code
	*/

	return "";
}

std::string Generator::generate_text_func_call(ASTNode node, int* length) {
	/*
		- generates machine code for a function call to be placed in .text segment

		node: ast node corresponding to the function call
		length: pointer to length variable for code length, will be updated after code has been generated
		rtype: generated code
	*/

	return "";
}

std::string Generator::generate_text_expr(ASTNode node, int* length) {
	/*
		- generates machine code for an expression to be placed in .text segment

		node: ast node corresponding to the expression
		length: pointer to length variable for code length, will be updated after code has been generated
		rtype: generated code
	*/

	return "";
}

std::string Generator::generate_text_cond(ASTNode node, int* length) {
	/*
		- generates machine code for conditional to be placed in .text segment

		node: ast node corresponding to the conditional
		length: pointer to length variable for code length, will be updated after code has been generated
		rtype: generated code
	*/
	
	return "";
}
