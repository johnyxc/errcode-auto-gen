#include "parser.h"
#include <time.h>
#include <vector>
#include <iostream>
#include <windows.h>

static void str2upper(std::string& str)
{
	if (str.empty()) return;
	for (auto& item : str)
		item = ::toupper(item);
}

static std::string get_year_month_day(time_t time)
{
	struct tm ret_tm = {};
	struct tm* tm_time = localtime(((time_t*)&time));

	ret_tm.tm_year = tm_time->tm_year + 1900;
	ret_tm.tm_mon = tm_time->tm_mon + 1;
	ret_tm.tm_mday = tm_time->tm_mday;

	char fmt[15] = {};
	sprintf(fmt, "%04d_%02d_%02d", ret_tm.tm_year, ret_tm.tm_mon, ret_tm.tm_mday);

	return fmt;
}

static std::vector<std::string> split(char* src, const char* delim)
{
	std::vector<std::string> word_list;

	auto res = strtok(src, delim);
	while (res != 0)
	{
		word_list.push_back(res);
		res = strtok(0, delim);
	}

	return word_list;
}
//////////////////////////////////////////////////////////////////////////

//	C++
void cpp_output_t::output(node_t* root)
{
	i_output(root);
	printf("Cpp Output Finish\n");
}

void cpp_output_t::i_output(node_t* node)
{
	if (!node) return;

	switch (node->node_type)
	{
	case NT_SEC:
		{
			switch (node->oper_type)
			{
			case OT_CREATE_FILE:
				{
					std::string fn = node->err_name + ".txt";
					file_ = fopen(fn.c_str(), "wb");
					if (!file_)
					{
						printf("Open File %s Error\n", fn.c_str());
						break;
					}
				}
				break;
			case OT_CLOSE_FILE:
				{
					if (file_) fclose(file_);
				}
				break;
			case OT_ENTER_SEC:
				// Do Nothing
				break;
			case OT_LEAVE_SEC:
				// Do Nothing
				break;
			default:
				printf("something wrong with syntax\n");
				break;
			}
		}
		break;
	case NT_EXP:
		{	//	只会写入
			char line[1024] = {};
			sprintf(line, "0x%x|%s\r\n", node->err_value, node->describe.c_str());
			fwrite(line, 1, strlen(line), file_);
		}
		break;
	default:
		printf("something wrong with syntax\n");
		break;
	}

	for (const auto& item : node->child)
	{
		i_output(item);
	}
}
//////////////////////////////////////////////////////////////////////////

//	Golang
void go_output_t::output(node_t* root)
{
	i_output(root);
	printf("Golang Output Finish\n");
}

void go_output_t::i_output(node_t* node)
{
	if (!node) return;

	switch (node->node_type)
	{
	case NT_SEC:
		{
			switch (node->oper_type)
			{
			case OT_CREATE_FILE:
				{
					std::string fn = node->err_name + ".go";
					file_ = fopen(fn.c_str(), "wb");
					if (!file_)
					{
						printf("Open File %s Error\n", fn.c_str());
						break;
					}

					auto* package = "// Errors\r\npackage Error\r\n";
					fwrite(package, 1, strlen(package), file_);
				}
				break;
			case OT_CLOSE_FILE:
				{
					if (err_msg_.size() != 0)
					{
						fwrite("\r\n", 1, strlen("\r\n"), file_);
						fwrite("// Error Messages\r\n", 1, strlen("// Error Messages\r\n"), file_);
						fwrite("var ErrorMsg = map[int]string{\r\n", 1, strlen("var ErrorMsg = map[int]string{\r\n"), file_);

						for (const auto& item : err_msg_)
						{
							char line[1024] = {};
							sprintf(line, "%s: \"%s\",\r\n", item.first.c_str(), item.second.c_str());
							fwrite(line, 1, strlen(line), file_);
						}

						fwrite("}\r\n", 1, strlen("}\r\n"), file_);
					}

					err_msg_.clear();
					if (file_) fclose(file_);
				}
				break;
			case OT_ENTER_SEC:
				{
					if (file_)
					{
						auto* exp = "const (";
						fwrite("\r\n/*", 1, strlen("\r\n/*"), file_);
						fwrite(node->describe.c_str(), 1, strlen(node->describe.c_str()), file_);
						fwrite("*/\r\n", 1, strlen("*/\r\n"), file_);
						fwrite(exp, 1, strlen(exp), file_);
					}
				}
				break;
			case OT_LEAVE_SEC:
				{
					if (file_)
					{
						auto* exp = "\r\n)\r\n";
						fwrite(exp, 1, strlen(exp), file_);
					}
				}
				break;
			default:
				printf("something wrong with syntax\n");
				break;
			}
		}
		break;
	case NT_EXP:
		{	//	只会写入
			char line[1024] = {};
			std::string err_name = node->err_prefix + "_" + node->err_name;
			str2upper(err_name);
			sprintf(line, "\r\n%s = 0x%x // %s", err_name.c_str(), node->err_value, node->describe.c_str());
			fwrite(line, 1, strlen(line), file_);

			err_msg_[err_name] = node->describe;
		}
		break;
	default:
		printf("something wrong with syntax\n");
		break;
	}

	for (const auto& item : node->child)
	{
		i_output(item);
	}
}
//////////////////////////////////////////////////////////////////////////

jf_cmd_parser_t* jf_cmd_parser_t::instance()
{
	static jf_cmd_parser_t self;
	return &self;
}

jf_cmd_parser_t::jf_cmd_parser_t() : output_(), parent_node_(), sibling_node_()
{
	root_ = new node_t;
	root_->node_type = NT_ROOT;
	parent_node_ = root_;
}
jf_cmd_parser_t::~jf_cmd_parser_t() {}

void jf_cmd_parser_t::set_target_type(const char* tar)
{
	if (!tar) return;

	if (g_tar_cpp == tar)
	{
		output_ = new cpp_output_t;
	}
	else if (g_tar_go == tar)
	{
		output_ = new go_output_t;
	}
}

void jf_cmd_parser_t::set_multiple_file(const char* tar)
{
	if (!tar) return;

	if (g_mul_files == tar)
	{
		multiple_files_ = true;
	}
	else
	{
		root_->node_type = NT_SEC;
		root_->oper_type = OT_CREATE_FILE;
		root_->err_name = "errors";		//	单文件默认名称 errors
	}
}

void jf_cmd_parser_t::set_global_comment(const char* cmt)
{	//	目前直接丢弃全局注释，可考虑写入单独的 Comment.h 文件中
}

void jf_cmd_parser_t::set_single_line_comment(const char* cmt, int line)
{
	if (!cmt) return;

	if (sibling_node_ == 0)
	{
		printf("something wrong with syntax\n");
		return;
	}

	if (sibling_node_->line == line)
	{
		//	1.对于 SECTION 类型节点，注释视为对此 SECTION 的注释
		//	2.对于 EXP 类型节点，注释视为错误码描述
		sibling_node_->describe = std::string(cmt + 2, strlen(cmt) - 4);
	}

	//	丢弃不属于任何错误描述的单行注释
}

void jf_cmd_parser_t::set_file_name(const char* cmt, int line)
{
	if (!cmt) return;

	if (!root_->child.empty())
	{
		auto leave_node = new node_t;
		leave_node->node_type = NT_SEC;
		leave_node->oper_type = OT_LEAVE_SEC;
		root_->child.push_back(leave_node);

		if (multiple_files_)
		{
			auto close_node = new node_t;
			close_node->node_type = NT_SEC;
			close_node->oper_type = OT_CLOSE_FILE;
			root_->child.push_back(close_node);
		}
	}

	parent_node_ = root_;

	if (multiple_files_)
	{
		auto node = new node_t;
		node->node_type = NT_SEC;
		node->oper_type = OT_CREATE_FILE;
		node->err_name = std::string(cmt + 1, strlen(cmt) - 2);
		node->line = line;
		if (parent_node_) parent_node_->child.push_back(node);
	}

	auto enter_node = new node_t;
	enter_node->node_type = NT_SEC;
	enter_node->oper_type = OT_ENTER_SEC;
	enter_node->err_name = std::string(cmt + 1, strlen(cmt) - 2);
	enter_node->line = line;
	if (parent_node_) parent_node_->child.push_back(enter_node);

	parent_node_ = sibling_node_ = enter_node;
}

void jf_cmd_parser_t::set_cmd_name(const char* cmt, int line, int sid)
{
	if (!cmt) return;

	auto node = new node_t;
	
	auto words = split(const_cast<char*>(cmt), " \t=;");
	if (words.size() == 1)
	{
		if (sibling_node_ == 0 || sibling_node_->node_type != NT_EXP)
		{
			printf("something wrong with syntax\n");
			return;
		}

		node->node_type = NT_EXP;
		node->oper_type = OT_WRITE;
		node->err_prefix = parent_node_->err_name;
		node->err_name = words[0];
		node->err_value = sibling_node_->err_value + 1;
		node->line = line;
	}
	else if (words.size() == 2)
	{
		node->node_type = NT_EXP;
		node->oper_type = OT_WRITE;
		node->err_prefix = parent_node_->err_name;
		node->err_name = words[0];
		node->line = line;
		auto res = sscanf(words[1].c_str(), "0x%x", &node->err_value);
		if (res == 0 || res == EOF)
		{
			sscanf(words[1].c_str(), "%d", &node->err_value);
		}
	}
	else
	{
		printf("something wrong with syntax\n");
		return;
	}

	sibling_node_ = node;
	if (parent_node_) parent_node_->child.push_back(node);
}

void jf_cmd_parser_t::output()
{
	auto node = new node_t;
	node->node_type = NT_SEC;
	node->oper_type = OT_LEAVE_SEC;
	root_->child.push_back(node);

	auto close_node = new node_t;
	close_node->node_type = NT_SEC;
	close_node->oper_type = OT_CLOSE_FILE;
	root_->child.push_back(close_node);
	
	if (output_) output_->output(root_);
}
