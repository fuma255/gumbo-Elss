// STHTMLPP.cpp: 定义控制台应用程序的入口点。
//
#pragma warning (disable : 4996)
#include "stdafx.h"
#include <iostream>
extern "C" {
	#include "sds.h"
	#include<Windows.h>  
}
#include<vector>
#include "gumbo.h"
using namespace std;
#ifdef HTMLAPI
# define HTMLAPI _declspec(dllexport)
#else
# define HTMLAPI _declspec(dllimport)
#endif

#define CALLTYPE _stdcall
#define HTMLAPI

#define HMBBAPI _stdcall

typedef struct HTML{

	HTMLAPI GumboOutput* CALLTYPE HTML_Create(const char* buffer)
	{
		return gumbo_parse(buffer);
	}
	HTMLAPI void CALLTYPE HTML_Destory(GumboOutput* output)
	{
		gumbo_destroy_output(&kGumboDefaultOptions, output);
	}
	HTMLAPI GumboNode* CALLTYPE HTML_GetRootNode(GumboOutput* output)
	{
		return output->root;
	}
	HTMLAPI GumboNode* CALLTYPE HTML_GetDocumentNode(GumboOutput* output)
	{
		return output->document;
	}

	//////////////////////////////////////////////////////////////
	HTMLAPI GumboNode* CALLTYPE HTMLNode_GetParent(GumboNode* node)
	{
		return node->parent;
	}
	HTMLAPI unsigned int CALLTYPE HTMLNode_GetIndexInParent(GumboNode* node)
	{
		return node->index_within_parent;
	}
	HTMLAPI GumboTag CALLTYPE HTMLNode_GetTag(GumboNode* node)
	{
		return node->v.element.tag;
	}
	HTMLAPI const char* CALLTYPE HTMLNode_GetOriginalTag(GumboNode* node)
	{
		return node->v.element.original_tag.data;
	}
	HTMLAPI unsigned int CALLTYPE HTMLNode_GetOriginalTagLength(GumboNode* node)
	{
		return node->v.element.original_tag.length;
	}
	HTMLAPI const char* CALLTYPE HTMLNode_GetOriginalEndTag(GumboNode* node)
	{
		return node->v.element.original_end_tag.data;
	}
	HTMLAPI unsigned int CALLTYPE HTMLNode_GetOriginalEndTagLength(GumboNode* node)
	{
		return node->v.element.original_end_tag.length;
	}

	HTMLAPI const char* CALLTYPE HTMLNode_GetText(GumboNode* node)
	{
		return node->v.text.text;
	}
	HTMLAPI GumboNodeType CALLTYPE HTMLNode_GetType(GumboNode* node)
	{
		return node->type;
	}
	HTMLAPI int CALLTYPE HTMLNode_GetChildCount(GumboNode* node)
	{
		return node->v.element.children.length;
	}
	HTMLAPI GumboNode* CALLTYPE HTMLNode_GetChild(GumboNode* node, size_t i)
	{
		return static_cast<GumboNode*>(node->v.element.children.data[i]);
	}
	HTMLAPI int CALLTYPE HTMLNode_GetAttributeCount(GumboNode* node)
	{
		return node->v.element.attributes.length;
	}
	HTMLAPI GumboAttribute* CALLTYPE HTMLNode_GetAttribute(GumboNode* node, size_t i)
	{
		return static_cast<GumboAttribute*>(node->v.element.attributes.data[i]);
	}

	HTMLAPI GumboAttribute* CALLTYPE HTMLNode_GetAttributeByName(GumboNode* node, const char* name)
	{
		return gumbo_get_attribute(&node->v.element.attributes, name);
	}

	/////////////////////////////////////////////////////////////////////////////////
	HTMLAPI const char* CALLTYPE HTMLAttribute_GetValue(GumboAttribute* attr)
	{
		return attr->value;
	}
	HTMLAPI const char* CALLTYPE HTMLAttribute_GetName(GumboAttribute* attr)
	{
		return attr->name;
	}
	/*GumboOutput*(*HTML_Create)(char*);
	GumboNode*(*HTML_GetRootNode)(GumboOutput*);
	GumboNode*(*HTML_GetDocumentNode)(GumboOutput*);
	GumboNode*(*HTMLNode_GetParent)(GumboNode*);
	unsigned int(*HTMLNode_GetIndexInParent)(GumboNode*);
	GumboTag(*HTMLNode_GetTag)(GumboNode*);
	const char*(*HTMLNode_GetOriginalTag)(GumboNode*);
	unsigned int(*HTMLNode_GetOriginalTagLength)(GumboNode*);
	
	const char*(*HTMLNode_GetOriginalEndTag)(GumboNode*);
	unsigned int(*HTMLNode_GetOriginalEndTagLength)(GumboNode*);
	const char*(*HTMLNode_GetText)(GumboNode*);
	GumboNodeType(*HTMLNode_GetType)(GumboNode*);
	int(*HTMLNode_GetChildCount)(GumboNode*);
	GumboNode *(*HTMLNode_GetChild)(GumboNode*,size_t);
	GumboAttribute*(*HTMLNode_GetAttributeCount)(GumboNode*, size_t);
	GumboAttribute*(*HTMLNode_GetAttributeByName)(GumboNode*, const char*);
	char*(*HTMLAttribute_GetValue)(GumboAttribute*);
	char*(*HTMLAttribute_GetName)(GumboAttribute*);

	void(*HTML_Destory)(GumboOutput*);*/
}HTMLengine;

typedef struct HtmlNode
{
	sds innerHtml;
	sds innerText;
}HtmlNode;

#define CLASS "class"
#define ID "id"

#define Fun_find "find"
#define Fun_fuzzyfind "fuzzyfind"


typedef std::vector<GumboNode*> GumboNodes;
typedef std::uint64_t hash_t;

constexpr hash_t prime = 0x100000001B3ull;
constexpr hash_t basis = 0xCBF29CE484222325ull;

string& trim(string &s)
{
	if (s.empty())
	{
		return s;
	}
	s.erase(0, s.find_first_not_of(" "));
	s.erase(s.find_last_not_of(" ") + 1);
	return s;
}

hash_t hash_(char const* str)
{
	hash_t ret{ basis };

	while (*str) {
		ret ^= *str;
		ret *= prime;
		str++;
	}

	return ret;
}

constexpr hash_t hash_compile_time(char const* str, hash_t last_value = basis)
{
	return *str ? hash_compile_time(str + 1, (*str ^ last_value) * prime) : last_value;
}

constexpr unsigned long long operator "" _hash(char const* p, size_t)
{
	return hash_compile_time(p);
}

vector<string> Split(char* path, const char* N)
{
	vector<string> Nodes;
	char *token;
	token = strtok(path, N);
	while (token != NULL) {
		string token1(token);
		Nodes.push_back(trim(token1));
		token = strtok(NULL, N);
	}
	delete[] token;
	return Nodes;
}

string strreplace(string str, string str1, string str2) {
	string::size_type idx = str.find(str1);	if (idx == string::npos)	return str;	str = str.replace(str.find(str1), str1.length(), str2);
return strreplace(str, str1, str2); }

void ergodic(vector<GumboNode*> *momo, HTMLengine* engine, GumboNode* Node, string path, string NodeType, BOOL recursion = true/*申明了递归深度true无限递归反之一层*/, BOOL layer = true)
{
	GumboNodeType Type = engine->HTMLNode_GetType(Node);
	if (Type == GUMBO_NODE_ELEMENT)
	{
		if (path.length() > 0)//防止里面啥都没有导致解析出错
		{
			if (NodeType.compare(Fun_find) == 0)
			{
				vector<string> Args = Split((char*)strreplace(path, "'", "").data(), ",");
				if (Args.size() == 2)
				{
					GumboAttribute* name = engine->HTMLNode_GetAttributeByName(Node, Args[0].c_str());
					if (name != NULL)
					{
						if (string(name->value).compare(Args[1]) == 0)
						{
							momo->push_back(Node);
						}
					}
				}
			}
			if (NodeType.compare(Fun_fuzzyfind) == 0)
			{
				vector<string> Args = Split((char*)strreplace(path, "'", "").data(), ",");
				if (Args.size() == 2)
				{
					GumboAttribute* name = engine->HTMLNode_GetAttributeByName(Node, Args[0].c_str());
					if (name != NULL)
					{
						if (string(name->value).find(Args[1]) != string::npos)
						{
							momo->push_back(Node);
						}
					}
				}
			}
			if (NodeType == "class")
			{
				GumboAttribute* name = engine->HTMLNode_GetAttributeByName(Node, NodeType.c_str());
				if (name != NULL)
				{
					if (string(name->value).compare(path) == 0)
					{
						momo->push_back(Node);
					}
				}
			}
			else if (NodeType == "id") {
				GumboAttribute* name = engine->HTMLNode_GetAttributeByName(Node, NodeType.c_str());
				if (name != NULL)
				{
					if (string(name->value) == path)
					{
						momo->push_back(Node);
					}
				}
			}
			else {
				const char* TagText = engine->HTMLNode_GetOriginalEndTag(Node);
				if (TagText != NULL)
				{
					string text(TagText);
					string text1 = text.substr(2, NodeType.size());
					if (string(text1) == NodeType)
					{
						momo->push_back(Node);
					}
				}
			}
			if (recursion)//控制递归深度
			{
				size_t n = engine->HTMLNode_GetChildCount(Node);
				for (size_t i = 0; i < n; i++)
				{
					GumboNode* nod = engine->HTMLNode_GetChild(Node, i);
					ergodic(momo, engine, nod, path, NodeType, layer);
				}
			}
		}
	} 
}

size_t FunEx(HTMLengine* engine, GumboNode* H,size_t te_1, string NodeName,vector<GumboNode*> &as,
	BOOL IsIndex
)
{//解析函数
	string text = NodeName.substr(te_1, NodeName.size() - te_1);
	vector<string> pats = Split((char*)NodeName.data(), ":");
	for (size_t i = 0; i < pats.size(); i++)
	{
		vector<GumboNode*> as_class;
		vector<string> list = Split((char*)pats.at(i).data(), "(");
		string data = list.size() > 0 ? list[0] : "";
		size_t j, x;
		string nam, nam_1;
		size_t te_1 = pats.at(i).find("[");
		if (te_1!=-1)
		{
			x = atoi(data.substr(te_1 + 1, data.find("]") - 1 - te_1).c_str());
			data = data.substr(0, te_1);
		}
		switch (hash_(data.c_str()))
		{
		case "find"_hash://属性筛选
			j = list[1].find(")");
			nam = list[1].replace(j, list[1].size() - j, "");
			if (as.size() > 0)
			{
				for (size_t i = 0; i < as.size(); i++)
				{
					ergodic(&as_class, engine, as.at(i), nam, Fun_find, true, i == 0 ? IsIndex : false);
				}
			}
			else {
				ergodic(&as_class, engine, H, nam, Fun_find, true, i == 0 ? IsIndex : false);
			}
			if (as_class.size() == 0)
			{
				return 0;//如果没有命中任何元素,则返回0
			}
			if (te_1 != -1)
			{
				if (x <= (as_class.size() - 1))
				{
					as.clear();
					as.push_back(as_class[x]);
				}
			}
			else {
				as.assign(as_class.begin(), as_class.end());
			}
			break;
		case "fuzzyfind"_hash://模糊筛选
			j = list[1].find(")");
			nam_1 = list[1].replace(j, list[1].size() - j, "");
			if (as.size() > 0)
			{
				for (size_t i = 0; i < as.size(); i++)
				{
					ergodic(&as_class, engine, as.at(i), nam_1, Fun_fuzzyfind, true, i == 0 ? IsIndex : false);
				}
			}
			else {
				ergodic(&as_class, engine, H, nam_1, Fun_fuzzyfind, true, i == 0 ? IsIndex : false);
			}
			if (as_class.size() == 0)
			{
				return 0;//如果没有命中任何元素,则返回0
			}
			if (te_1 != -1)
			{
				if (x <= (as_class.size() - 1))
				{
					as.clear();
					as.push_back(as_class[x]);
				}
			}
			else {
				as.assign(as_class.begin(), as_class.end());
			}
			break;
		default://如果没有命中函数则是标签
			if (as.size() > 0)
			{
				for (size_t i = 0; i < as.size(); i++)
				{
					ergodic(&as_class, engine, as.at(i), data, data, true, i == 0 ? IsIndex : false);
				}
			}
			else {
				ergodic(&as_class, engine, H, data, data, true, i == 0 ? IsIndex : false);
			}
			if (as_class.size() == 0)
			{
				return 0;//如果没有命中任何元素,则返回0
			}
			if (te_1 != -1)
			{
				if (x <= (as_class.size() - 1))
				{
					as.clear();
					as.push_back(as_class[x]);
				}
			}
			else {
				as.assign(as_class.begin(), as_class.end());
			}
			break;
		}
	}
}

int extract_nodes(HTMLengine* engine, GumboNode* H, vector<GumboNode*> &as,string NodeName, string NodeType)
{
	vector<GumboNode*> as_class;
	//ergodic(&as_class, engine, H, "a", "a");
	size_t te = NodeName.find(":");
	if (te == -1)
	{
		size_t te_1 = NodeName.find("[");
		if (te_1 != -1)
		{
			size_t x = atoi(NodeName.substr(te_1 + 1, NodeName.find("]") - 1 - te_1).c_str());
			if (as.size() > 0)
			{
				for (size_t ia = 0; ia < as.size(); ia++)
				{
					ergodic(&as_class, engine, as.at(ia), NodeName.substr(0, te_1), NodeType, true, false);
				}
			}
			else {
				ergodic(&as_class, engine, H, NodeName.substr(0, te_1), NodeType);
			}
			if (as_class.size() == 0)
			{
				return 0;//如果没有命中任何元素,则返回0
			}
			if (x <= (as_class.size() - 1))
			{
				as.clear();
				as.push_back(as_class[x]);
			}
		}
		else {
			if (as.size() > 0)
			{
				for (size_t i1 = 0; i1 < as.size(); i1++)
				{
					ergodic(&as_class, engine, as.at(i1), NodeName, NodeType, true, false);
				}
			}
			else {
				ergodic(&as_class, engine, H, NodeName, NodeType);
			}
			if (as_class.size() == 0)
			{
				return 0;//如果没有命中任何元素,则返回0
			}
			as.assign(as_class.begin(), as_class.end());
		}
	}
	else {
		//--解析表达式
		size_t te_1 = te;
		te_1++;
		if (as.size() > 0)
		{
			for (size_t i = 0; i < as.size(); i++)
			{
				FunEx(engine, as[i], te_1, NodeName, as, false);
			}
		}
		else {
			FunEx(engine, H, te_1, NodeName, as, true);
		}
		if (as_class.size() == 0)
		{
			return 0;//如果没有命中任何元素,则返回0
		}
		te_1 = NodeName.find("[");
		if (te_1 != -1) {
			size_t x = atoi(NodeName.substr(te_1 + 1, NodeName.find("]") - 1 - te_1).c_str());
			if (x <= (as_class.size() - 1))
			{
				as.clear();
				as.push_back(as_class[x]);
			}
		}
		else { as.assign(as_class.begin(), as_class.end()); }
		//--
	}
	return 1;
}

vector<GumboNode*> CLHQ(HTMLengine* engine, GumboNode* H,char* path, vector<GumboNode*>* list = NULL)
{
	//".class > a > div > .s"
	//"#id > a > div > .s"
	//"div > a > div > .s"
	//首先解析表达式
	vector<string> Nodes;
	char *token;
	const char s[2] = ">";
	token = strtok(path, s);
	while (token != NULL) {
		string token1(token);
		Nodes.push_back(trim(token1));
		token = strtok(NULL, s);
	}
	vector<GumboNode*> as;
	if (list != NULL)
		as.assign(list->begin(), list->end());
	for (size_t i = 0; i < Nodes.size(); i++)
	{
		char a = Nodes[i].c_str()[0];
		switch (a)
		{
		case '.'://遍历as节点数组寻找满足class节点
			extract_nodes(engine,H, as,Nodes[i].erase(0, 1), CLASS);
			break;
		case '#'://遍历as节点数组寻找满足id节点
			extract_nodes(engine, H, as, Nodes[i].erase(0, 1), ID);
			break;
		default://遍历as节点数组寻找满足节点	
			extract_nodes(engine, H, as, Nodes[i], (Nodes[i].find("[")!=-1)? Nodes[i].substr(0, Nodes[i].find("[")): Nodes[i]);
			break;
		}
	}
	return as;
}

HTMLAPI HTMLengine* HMBBAPI CreateHtml()
{
	return (HTMLengine *)malloc(sizeof(HTMLengine));
}
//GB2312到UTF-8的转换
char* G2U(const char* gb2312)
{
	int len = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr) delete[] wstr;
	return str;
}
//UTF-8到GB2312的转换
char* U2G(const char* utf8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
	len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr) delete[] wstr;
	return str;
}
HTMLAPI void HMBBAPI engineFree(HTMLengine* e, GumboOutput *engine);
void STHTML(sds HTML,sds path)
{
	//初始化所有接口api
	HTMLengine *html = CreateHtml();
	//加载解析一个html字符串
	GumboOutput *engine = html->HTML_Create(HTML);
	//获取html根节点
	GumboNode* gum = html->HTML_GetRootNode(engine);
	//根据自定义css选择器函数解析一个节点或者节点数组
	vector<GumboNode*> HTMLnode = CLHQ(html, gum, path);
	//Source.def
	//返回节点数组
	//切记不能销毁引擎之后再去读节点 会爆炸的
	cout << "表达式: "<< "#list > dl > dd > a" <<"\n";
	for (size_t i = 0; i < HTMLnode.size(); i++)
	{
		//cout << HTMLnode[i]->v.element.children.length << "\n";
		GumboNode* htmlcode = html->HTMLNode_GetChild(HTMLnode[i], 0);
		std::string no(HTMLnode[i]->v.element.original_tag.data, HTMLnode[i]->v.element.original_end_tag.data- HTMLnode[i]->v.element.original_tag.data+ HTMLnode[i]->v.element.original_end_tag.length);
		//std::string eo(, HTMLnode[i]->v.element.original_end_tag.length);
		std::cout << U2G(no.c_str()) << std::endl;
		cout << "结果:" << U2G(htmlcode->v.text.text) << "\n";
	}
	//销毁引擎
	engineFree(html, engine);
	//html->HTML_Destory(engine);
}
#include <fstream>
int main()
{
	const char* filename = "utf8.html";

	std::ifstream in(filename, std::ios::in | std::ios::binary);
	if (!in) {
		std::cout << "File " << filename << " not found!\n";
		exit(EXIT_FAILURE);
	}

	std::string contents;
	in.seekg(0, std::ios::end);
	contents.resize(in.tellg());
	in.seekg(0, std::ios::beg);
	in.read(&contents[0], contents.size());
	in.close();

	sds htmla = sdsnew(contents.c_str());
	sds patha = sdsnew("div:find('class','book-info'):h4:a[0]");//a:find('data-eid','qd_F24')[1]

	//目前表达式很简单
	//.class > a > div > li
	//.class > #kk > div > li
	//#id > #kk > div > li
	//div > a[5] > li > p[1]
	STHTML(htmla, patha);
    return 0;
}

HTMLAPI GumboOutput* HMBBAPI LoadHtml(HTMLengine* html,const char* HTML)
{
	//throw "sss";
	//加载解析一个html字符串
	return html->HTML_Create(HTML);
}
HTMLAPI GumboNode* HMBBAPI GetRootNode(HTMLengine* html, GumboOutput *engine)
{
	return html->HTML_GetRootNode(engine);
}
HTMLAPI LPVOID HMBBAPI Find(HTMLengine *html, GumboNode* gum,char* path)
{
	GumboNodes *nodes = new GumboNodes;
	vector<GumboNode*> asd = CLHQ(html, gum, path);
	nodes->assign(asd.begin(), asd.end());
	return nodes;
}
HTMLAPI LPVOID HMBBAPI Find1(HTMLengine *html, GumboNode* gum, vector<GumboNode*> *nodes,char* path)
{
	GumboNodes *nodess = new GumboNodes;
	vector<GumboNode*> asd = CLHQ(html, gum, path, nodes);
	nodess->assign(asd.begin(), asd.end());
	return nodess;
}
HTMLAPI void HMBBAPI engineFree(HTMLengine* e, GumboOutput *engine)
{
	e->HTML_Destory(engine);
}
HTMLAPI void HMBBAPI engineClose(HTMLengine* e)
{
	free(e);
}
HTMLAPI GumboNode* HMBBAPI GetNode(vector<GumboNode*> &asd,size_t i)
{
	if (i>=asd.size())return NULL;
	return asd[i];
}
HTMLAPI int HMBBAPI GetNodeSize(vector<GumboNode*> &asd)
{
	if (asd.size() <= 0)return 0;
	return asd.size();
}
HTMLAPI const char* HMBBAPI GetNodeText(HTMLengine* html,GumboNode* htmlcode, BOOL isU_G)
{
	try
	{
		std::string no(html->HTMLNode_GetChild(htmlcode, 0)->v.text.text);
		return (isU_G) ? U2G(no.length()==0?"null":no.c_str()) : no.c_str();
	}
	catch (const std::exception&)
	{
		return "\0";
	}
	
}
HTMLAPI const char* HMBBAPI GetNodeHtml(GumboNode* htmlcode,BOOL isU_G)
{
	try
	{
		std::string no(htmlcode->v.element.original_tag.data, htmlcode->v.element.original_end_tag.data - htmlcode->v.element.original_tag.data + htmlcode->v.element.original_end_tag.length);
		return (isU_G) ? U2G(no.c_str()) : no.c_str();
	}
	catch (const std::exception&)
	{
		return "\0";
	}
}
HTMLAPI const char* HMBBAPI GetAttr(GumboNode* htmlcode,const char* name)
{
	try
	{
		return gumbo_get_attribute(&htmlcode->v.element.attributes, name)->value;
	}
	catch (const std::exception&)
	{
		return "\0";
	}
}

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
#include <Windows.h>
BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}