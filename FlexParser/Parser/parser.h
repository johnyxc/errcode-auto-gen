#ifndef __PARSER_H_2023_01_11__
#define __PARSER_H_2023_01_11__
#include <map>
#include <list>
#include <string>
#include <functional>

static const std::string g_tar_cpp		= "-cpp";
static const std::string g_tar_go		= "-go";
static const std::string g_mul_files	= "-mul";
static const std::string g_sig_file		= "-sig";

enum NODE_TYPE {
	NT_ROOT = 1,	//	根节点
	NT_SEC,			//	节标志
	NT_EXP			//	表达式
};

enum OPER_TYPE {
	OT_CREATE_FILE = 1,
	OT_CLOSE_FILE,
	OT_ENTER_SEC,
	OT_LEAVE_SEC,
	OT_WRITE,
};

struct node_t
{
	NODE_TYPE			node_type;
	OPER_TYPE			oper_type;
	std::string			err_prefix;	//	错误码前缀
	std::string			err_name;	//	错误码名称
	unsigned int		err_value;	//	错误码值
	std::string			describe;	//	错误描述
	int					line;		//	行号，主要用于定位描述
	std::list<node_t*>	child;
};
//////////////////////////////////////////////////////////////////////////

struct target_output_intf
{
	virtual void output(node_t* root) = 0;
};

//	C++ 文件输出
struct cpp_output_t :
	target_output_intf
{
public:
	void output(node_t* root) override;

private:
	void i_output(node_t* node);

private:
	std::list<node_t*> data_nodes_;
	FILE* file_;
};

//	Golang 文件输出
struct go_output_t :
	target_output_intf
{
public:
	void output(node_t* root) override;

private:
	void i_output(node_t* node);

private:
	std::list<node_t*> data_nodes_;
	std::map<std::string, std::string> err_msg_;
	FILE* file_;
};
//////////////////////////////////////////////////////////////////////////

struct jf_cmd_parser_t
{
public:
	static jf_cmd_parser_t* instance();
	~jf_cmd_parser_t();

public:
	void set_target_type(const char* tar);
	void set_multiple_file(const char* tar);

	void set_global_comment(const char* cmt);
	void set_single_line_comment(const char* cmt, int line);
	void set_file_name(const char* cmt, int line);
	void set_cmd_name(const char* cmt, int line, int sid);

	void output();

private:
	jf_cmd_parser_t();

private:
	node_t*				root_;
	node_t*				parent_node_;
	node_t*				sibling_node_;
	target_output_intf* output_;
	bool				multiple_files_;
};

#endif
