#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <bitset>
#include <math.h>
#include <time.h>
#include <Windows.h>
using namespace std;
template<size_t N>
class MyNum
{
public:
	bitset<N>b;

	void set(int ary[], int nSize)
	{
		b.reset();
		for (int i=0; i<nSize; i++)
		{
			b.set(ary[i]-1, 1);
		}
	}
};

int mainx()
{
	int a[6][10] = {
		{1,2,3,4,5,6,7,8,9,10},
		{1,3,5,7,8,9,12,13,14,15},
		{1,2,3,4,5,6,7,8,9,10},
		{1,3,5,7,8,9,12,13,14,15},
		{1,2,3,4,5,6,7,8,9,10},
		{1,3,5,7,8,9,12,13,14,15}
	};

	ofstream out("d:\\data.txt");
	MyNum<24> m;
	for (int i=0; i<6; i++)
	{
		m.set(a[i], 10);
		out.write((char*)&m.b, 3);
		cout << i << "\t";
	}
	cout << endl;
	out.close();

	ifstream in("d:\\data.txt");
	bitset<24>b;
	while (!in.eof())
	{
		in.read((char * )&b, 3);
		for (int i=0; i<24; i++)
		{
			if (b.test(i))
			{
				cout << i+1 << "\t";
			}
		}
		cout << endl;
	}
	in.close();

	system("pause");
	return 0;
}

void mainxy()
{
	vector<string> vInstr;
	string aa = "e12  de23 dd3 d34";
	char *ax[] = {"a", "a"};
	istringstream istr(aa);
	string qq;
	while (istr >> qq)
	{
		cout << qq << endl;
		vInstr.push_back(qq);
	}

	cout << "----------------------------" << endl;

	vector<string>::iterator vit;
	for (vit = vInstr.begin(); vit != vInstr.end(); vit++)
	{
		cout << *vit << endl;
	}

	cout << "----------------------------" << endl;
	cout << vInstr[0].c_str() << endl;

	system("pause");
}



#include <stdio.h>
#include <conio.h> 
int mainxxx()
{
	int a=1,b=2,c;
	while(getch()!=0x1b)   //改在这里判断
	{
		c=a+b;
		printf("%d",c);   //没次打印一个3，直到输入ESC
	}
	return 0;
}

#define Random(X) (rand()%X)

int maindd()
{
	double x=12.1;
	double y = 3.0;
	std::cout << fmod(x,y) << std::endl;

	std::cout << 0.08 / 100 << std::endl;

	srand((int)time(0));  //设置随机种子
	if (Random(2) == 1)
	{
		std::cout << " test1 " << std::endl;
	}
	else
	{
		std::cout << " test0 " << std::endl;
	}

	for (int i=0; i<10; i++)
	{
		std::cout << Random(2) << std::endl;
	}

	system("pause");
	return 0;
}

void timemain()
{
	int timeout = 10;
	DWORD dwStart = GetTickCount(); //取windows启动到现在的流逝时间(毫秒)

	std::cout << dwStart << std::endl;

	int x = 0;
	x = -x;
	std::cout << x << std::endl;

	system("pause");
}

#include <sstream>
void mainQQ()
{
	ifstream fin( "../cfg/cases.cfg" ); 
	string line;
	string s;
	while (getline(fin, line))
	{
		if (line == "")
			break;
		std::stringstream w(line);
		//cout << line << endl;
		w >> s;
		cout << s << endl;
	}

	system("pause");
}


bool
	strict_weak_ordering(const std::pair<int,std::string> a, const std::pair<int,std::string>  b)    

{      
	return a.first < b.first;      
}      

int main()      
{      
	using namespace std;      

	 vector<pair<int, string> > vec;      

		vec.push_back(make_pair<int, string>(5, "5hello"));      
		vec.push_back(make_pair<int, string>(4, "4hell"));      
		vec.push_back(make_pair<int, string>(6, "6hello,"));      

		//sort(vec.begin(), vec.end(), strict_weak_ordering);      

		vector<pair<int, string> >::iterator it = vec.begin(), end = vec.end();      

		for(; it != end; ++it)      
			cout<<it->second<<endl;
	system("pause");  
		return 0;        
}