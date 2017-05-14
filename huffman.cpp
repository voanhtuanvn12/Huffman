// Võ Anh Tuấn
// 1512641
// Thank to https://github.com/TheNilesh/huffman

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <fstream>
using namespace std;
#define MAX 16	//MAXimum length of codeword, more the MAX more overhead
char padding;
unsigned char N;

typedef struct symCode
{
	char x;
	char code[MAX];
}symCode;

symCode *symlist;
int n;

#define INTERNAL 1
#define LEAF 0


typedef struct node // nút
{
	char x; // lưu ký tự x
	int freq; // lưu số lần xuát hiện
	char *code; // lưu mã sau khi nén
	int type; // lưu kiểu để xác định có phải nút lá hay không
	struct node *next; // con trỏ đến vị trí kế tiếp
	struct node *left; // con trỏ đến cây con trái
	struct node *right;// con trỏ đến cây con phải
}node; // nút

node *HEAD, *ROOT;
fstream fp, fp2;

node* newNode(char c)
{
	node *q;
	q = new node;
	q->x = c;
	q->type = LEAF;	//leafnode
	q->freq = 1;
	q->next = NULL;
	q->left = NULL;
	q->right = NULL;
	return q;
}
void addSymbol(char c);
void insert(node *p, node *m);// chèn nút p vào nút m
void makeTree();// tạo cây huffman
void genCode(node *p, char* code);
void writeHeader(); // ghi
void writeBit(int b); // ghi bit
void writeCode(char ch); // ghi code đã được nén
char *getCode(char ch); // lấy mã huffman của ký tự ch
//###############################################################
int fileError();
char *decodeBuffer(char b); 
char *int2string(int n); 
int match(char a[], char b[], int limit);
int main()
{
	string a, b;
	char ch;
	char input[100], output[100];
	//char *input = "input.txt";
	//char *output = "output.txt";
	int choose = 2;
	if (choose == 1)
	{
		cout << "Nhap tap tin input : " << endl;
		//cin.ignore();
		cin >> a;
		cout << "Nhap tap tin output : " << endl;
		//cin.ignore();
		cin >> b;
		strcpy(input, a.c_str());
		strcpy(output, b.c_str());
		HEAD = NULL;
		ROOT = NULL;

		fp.open(input, ios::in | ios::binary);
		if (!fp.good())
		{
			cout << "\nKhong the mo file input!!" << endl;
			return -1;
		}

		cout << "\nDang doc tap tin input" << endl;
		while (fp.read(&ch, sizeof(char))) // chuyển ký tự vào trong node
		{
			addSymbol(ch);
		}
		fp.close();
		cout << "\nDoc tap tin xong ! " << endl;
		cout << "\nDang xay dung cay huffman..." << endl;
		makeTree();
		cout << "\nTao cay thanh cong !" << endl;
		cout << "\nDang xay dung ma huffman cho tung node la...." << endl;
		genCode(ROOT, "\0");
		cout << "\nDa tao thanh cong!" << endl;

		// đọc lại tập input
		fp.open(input, ios::in);
		if (!fp.good()) // nếu không mở được
		{
			cout << "\nKhong the mo file input!!" << endl;
			return -1;
		}
		fp2.open(output, ios::out | ios::binary);
		if (!fp2.good()) // nếu không mở được
		{
			cout << "\nKhong the mo file output!!" << endl;
			return -1;
		}

		cout << "\nDang doc file input" << endl;
		cout << "\nDang nen vao file output\n";
		cout << "\nDang ghi phan header (cay huffman)\n";
		writeHeader();
		cout << "\nDa ghi xong cay huffman vao file" << endl;
		cout << "\nDang ghi noi dung nen" << endl;
		while (fp.read((char*)&ch, sizeof(char)))
		{
			writeCode(ch);
		}
		fp.close();
		fp2.close();
		cout << "Da nen xong" << endl;
	}
	else if (choose == 2)
	{
		cout << "Nhap tap tin nen : " << endl;
		//cin.ignore();
		cin >> a;
		cout << "Nhap tap tin output : " << endl;
		//cin.ignore();
		cin >> b;
		strcpy(input, a.c_str());
		strcpy(output, b.c_str());
		char buffer;
		char *decoded = NULL;
		int i;
		fp.open(input, ios::in | ios::binary);
		if (!fp.good())
		{
			cout << "\nKhong the mo file input!!" << endl;
			return -1;
		}
		//Đọc phần header của file
		cout << "\nDang doc phan header " << endl;
		if (!fp.read((char*)&buffer, sizeof(char)))
			return fileError(); // lỗi đọc file
		N = buffer;		//No. of structures(mapping table records) to read
		if (N == 0)
			n = 256;
		else
			n = N;
		cout << "\n Co" << n << " ky tu khac nhau trong file" << endl;
		// cấp phát vùng nhớ cho bảng huffman
		symlist = new symCode[n];
		cout << "\nDang doc bang huffman trong file" << endl;
		if (!fp.read((char*)symlist, n * sizeof(symCode)))
			return fileError();
		if (!fp.read((char*)&buffer, sizeof(char))) return (fileError());
		padding = buffer;		//No. of bits to discard
		cout << "\n Tim thay " << (int)padding << " bit thua` " << endl;
		fp2.open(output, ios::out | ios::binary);
		if (!fp2.good()) // nếu không mở được
		{
			cout << "\nKhong the mo file output!!" << endl;
			return -2;
		}
		cout << "\nDang giai nen" << endl;
		while (fp.read((char*)&buffer, sizeof(char)))
		{
			decoded = decodeBuffer(buffer);
			i = 0;
			while (decoded[i++] != '\0');	//i-1 characters read into decoded array
			fp2.write((char*)decoded, (i - 1) * sizeof(char));
		}
		fp.close();
		fp2.close();
		cout << "\n	Da giai nen thanh cong !!!" << endl;
	}
	return 0;
}
void insert(node *p, node *m) // chèn nút p vào nút m
{
	// chèn dựa trên số lần xuất hiện của ký tự
	if (m->next == NULL)
	{
		m->next = p;
		return;
	}
	while (m->next->freq < p->freq)
	{
		m = m->next;
		if (m->next == NULL)
		{
			m->next = p;
			return;
		}
	}
	p->next = m->next;
	m->next = p;
}
void addSymbol(char c)
{
	// Chèn ký tự vào danh sách liên kết nếu chưa có, ngược lại thì cộng số lần xuất hiện
	node *p, *q, *m;
	if (HEAD == NULL) // nếu danh sách rỗng
	{
		HEAD = newNode(c);
		return;
	}
	// nếu danh sách không rỗng
	p = HEAD; q = NULL;
	if (p->x == c) // nếu nút chứa kí tự đã có
	{
		p->freq++; // cộng số lần xuát hiện lên 1
		if (p->next == NULL) // nếu không có node nào ở phía sau
		{
			return;
		}
		if (p->freq > p->next->freq)
		{
			HEAD = p->next;
			p->next = NULL;
			insert(p, HEAD);
		}
		return;
	}
	// nếu nút ko chứa ký tự 
	// thì xét nút ->pnext;

	while (p->next != NULL && p->x != c)
	{
		q = p;
		p = p->next;
	}
	// nếu tìm thấy nút p mà p.x = c
	if (p->x == c)
	{
		p->freq++; // tăng biến đếm
		if (p->next == NULL)
			return;
		if (p->freq > p->next->freq) // xét lại vị trí
		{
			m = p->next;
			q->next = p->next;
			p->next = NULL;
			insert(p, HEAD);
		}
	}
	else  //p->next==NULL 
	{
		q = newNode(c);
		q->next = HEAD; // chèn vào đầu danh sách vì fre luôn bằng 1
		HEAD = q;
	}
}
void makeTree()
{
	node  *p, *q = NULL;
	p = HEAD;
	while (p != NULL) // chạy đến khi hết danh sách
	{
		q = newNode('@');// ký tự đặt biệt
		q->type = INTERNAL; // báo hiệu không phải node lá
		q->left = p;		// gán cây con trái của q là p
		q->freq = p->freq;
		if (p->next != NULL) // nếu p->next không rỗng
		{
			p = p->next;
			q->right = p;// gán cây con phải của q là p
			q->freq += p->freq; // cập nhật  fre của nút gốc
		}
		p = p->next;
		if (p == NULL) // nếu đã hết ds
			break;
		// ngược lại nếu chưa hết ds
		// chèn node mới 
		if (q->freq <= p->freq)
		{
			q->next = p;
			p = q;
		}
		else
			insert(q, p);
	}
	ROOT = q; // gán cho ROOT;
}
void genCode(node *p, char* code)
{
	char *lcode, *rcode;
	static node *s;
	static int flag;
	if (p != NULL) // khi chưa hết danh sách
	{
		// nếu là lá
		if (p->type == LEAF)
		{
			if (flag == 0) //first leaf node
			{
				flag = 1; HEAD = p;
			}
			else	//other leaf nodes
			{
				s->next = p;
			}		//sorting LL
			p->next = NULL;
			s = p;
		}
		// tạo mã 
		p->code = code;
		lcode = new char[strlen(code) + 2];
		rcode = new char[strlen(code) + 2];
		sprintf(lcode, "%s0", code);
		sprintf(rcode, "%s1", code);
		// đệ quy
		genCode(p->left, lcode);
		genCode(p->right, rcode);
	}
}
void writeHeader() // ghi
{
	symCode record;
	node* p;
	int temp = 0, i = 0;
	p = HEAD;
	while (p != NULL)
	{
		temp += (strlen(p->code)) * (p->freq);
		if (strlen(p->code) > MAX) printf("\n[!] Ma code qua dai");
		temp %= 8;
		i++;
		p = p->next;
	}

	if (i == 256)
		N = 0;	//if 256 diff bit combinations exist, then alias 256 as 0
	else
		N = i;

	fp2.write((char*)&N, sizeof(unsigned char));
	cout << "N = " << i << endl;
	p = HEAD;
	while (p != NULL)
	{
		record.x = p->x;
		strcpy(record.code, p->code);
		fp2.write((char*)&record, sizeof(symCode));
		p = p->next;
	}
	padding = 8 - (char)temp;	//int to char & padding = 8-bitsExtra
	fp2.write((char*)&padding, sizeof(char));
	cout << "\nPadding = " << (int)padding << endl;
	//do actual padding
	for (i = 0; i < padding; i++)
		writeBit(0);
}
void writeBit(int b) // ghi bit
{
	static char byte; // khi đủ 8 bit thì sẽ ghi byte lên file
	static int cnt; // biến đếm
	char temp;
	if (b == 1) // nếu bit bằng 1
	{
		temp = 1;
		temp = temp << (7 - cnt);		//right shift bits
		byte = byte | temp;
	}
	cnt++;
	if (cnt == 8)	//buffer full
	{
		fp2.write((char*)&byte, sizeof(char));
		cnt = 0; byte = 0;	//reset buffer
		return;// buffer written to file
	}
	return;
}
char *getCode(char ch)
{
	node *p = HEAD;
	while (p != NULL)
	{
		if (p->x == ch)
			return p->code;
		p = p->next;
	}
	return NULL; //not found
}
void writeCode(char ch)
{
	char *code;
	code = getCode(ch);
	while (*code != '\0')
	{
		if (*code == '1')
			writeBit(1); //write bit 1 into file f
		else
			writeBit(0);
		code++;
	}
	return;
}
///############################################################
int fileError()
{
	printf("[!]Loi doc file.\n[ ]Khong phai file nen dung thuan toan huffman.\n");
	fp.close();
	return -3;
}
char *int2string(int n)
{
	int i, k, and, j;
	char *temp = (char *)malloc(16 * sizeof(char));
	j = 0;

	for (i = 15; i >= 0; i--)
	{
		and = 1 << i;
		k = n & and;
		if (k == 0) temp[j++] = '0'; else temp[j++] = '1';
	}
	temp[j] = '\0';
	return temp;
}
int match(char a[], char b[], int limit)
{
	b[strlen(a)] = '\0';
	b[limit] = '\0';
	return strcmp(a, b);
}
char *decodeBuffer(char b)
{
	int i = 0, j = 0, t;
	static int k;
	static int buffer;	// buffer larger enough to hold two b's
	char *decoded = new char[MAX];
	/*
	Logic:
	buffer = [1 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0]
	k
	b   =        [ 1 0 1 1 0 0 1 1 ]
	//put b in integer t right shift k+1 times then '&' with buffer; k=k+8;
	buffer = [1 0 0 1 1 0 1 1 0 0 1 1 0 0 0 0]
	k
	*/

	t = (int)b;
	//printf("\nt=%sk=%d",int2string(t),k);
	t = t & 0x00FF;		//mask high byte
						//printf("\nt=%sk=%d",int2string(t),k);
	t = t << 8 - k;		//shift bits keeping zeroes for old buffer 
						//printf("\nt=%sk=%d",int2string(t),k);
	buffer = buffer | t;	//joined b to buffer
	k = k + 8;			//first useless bit index +8 , new byte added

	if (padding != 0)	// on first call
	{
		buffer = buffer << padding;
		k = 8 - padding;	//k points to first useless bit index
		padding = 0;
	}

	//printf("\nbuffer=%s, k=%d",int2string(buffer),k);
	//loop to find matching codewords

	while (i < n)
	{
		if (!match(symlist[i].code, int2string(buffer), k))
		{
			decoded[j++] = symlist[i].x;	//match found inserted decoded
			t = strlen(symlist[i].code);	//matched bits
			buffer = buffer << t;		//throw out matched bits
			k = k - t;				//k will be less
			i = 0;				//match from initial record
								//printf("\nBuffer=%s,removed=%c,k=%d",int2string(buffer),decoded[j-1],k);
			if (k == 0) break;
			continue;
		}
		i++;
	}

	decoded[j] = '\0';
	return decoded;

}//fun
