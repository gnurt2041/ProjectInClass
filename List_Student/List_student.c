// Code by thanhtrung1.4

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Cau truc du lieu cua bai taon
struct Ngay{
	int ngay, thang, nam;
};
typedef struct SinhVien
{
	char maSV[8];
	char hoTen[50];
	int gioiTinh;
	struct Ngay ngaySinh;
	char diaChi[100];
	char lop[12];
	char khoa[7];
	
}sinhvien;
typedef struct Node{
	
    sinhvien data;
	struct Node *link;
	
}*node;
typedef struct List{
	
	node first;
	node last;
	
}list;

// Cac ham xu li list
node Create(sinhvien dulieu ) // ham tao node
{
	node temp;
	temp = (node)malloc(sizeof(struct Node));// can cap phat vung nho cho node moi
	temp->data = dulieu;// du lieu cua temp trong node = du lieu
	temp->link = NULL;// con tro link cua temp tro den NULL
	return temp;
}
node addtail( node first , node temp ) // ham them vao cuoi list
{
	node pi;
	if ( first == NULL ){ // danh sach dang rong
		first = temp; // temp se la first
	}
	else
	{
	pi = first; // khoi tao pi bat dau tu first
	while( pi->link != NULL ) 
		{
			pi = pi->link; 
		}
		pi->link = temp; 
	    // temp se la ptu cuoi cung moi nen pi->p se tro den temp
	}
	return first;
}
int size(node first) // ham lay kich thuoc list
{
	node px = first;// khoi tao node px
	int k;
	while(px != NULL ){
		k++;
		px = px->link;
	}
	return k;
}
void swap(node x, node y) // ham doi cho 2 node 
{
	node t = Create(x->data);
	x->data = y->data;
	y->data = t->data;
}
node delAtT( node first,int T ) // ham xoa node o vi tri T
{
	node x = first;
	int k = 0;
	if( T == 0 ) { // xoa o vi tri dau
		first = x->link;
	    return first;
	}
	if( T > size(first) ) return first;// neu T o ngoai list thi khong xoa gi
	while( x != NULL && k!=(T-1) ) { // duyet tim vi tri T
		x = x->link;
	    k++;
	}
	x->link = x->link->link;// xao node bang cai thay doi link cua node truoc no
	return first;
}
int search(node first, int value) // ham tim vi tri cua node, gia tri ngay sinh la value
{
	int ps = 0;
	node px;
	for(px = first; px != NULL; px = px->link){ // duyet tim vi tri can tim
        if(px->data.ngaySinh.ngay == value){
            return ps;
        }
        ++ps;
    }
    return -1;
}

// Cac ham xu ly cua bai toan
node delBirthday( node first,int value ) // ham xoa nhung sinh vien cung ngay sinh
{
	int position = search(first, value); // tim vi tri cua sinh vien co cung ngay sinh trung 
	while(position != -1){               // xoa tat ca sinh vien co cung ngay sinh
		first = delAtT(first, position);
		position = search(first, value);
	}
	return first;
}
node sort(node first) // ham sap xep list theo ma sinh vien
{
	node pi;
	for(pi = first; pi->link != NULL; pi = pi->link){ // duyet tu dau list den phan tu cuoi -1
		int x = atoi(pi->data.maSV); // chuyen char maSV sang int maSV de so sanh
		node pj;
		for(pj = pi->link; pj != NULL; pj = pj->link){ // duyet tu dau list den cuoi list
			int y = atoi(pj->data.maSV);
			if( x > y ){ // maSV cua pi > maSV cua pj
				swap(pi, pj);// doi cho 2 node
			}
		}
    }
    return first;
}
int check(node first) // ham kiem tra sinh vien cung ngay sinh
{
	node pi;
	for(pi = first; pi->link != NULL; pi = pi->link){ // duyet list 	
			node pj;
		    int x = pi->data.ngaySinh.ngay;
		    for(pj = pi->link; pj != NULL; pj = pj->link)
			{
				int y = pj->data.ngaySinh.ngay;
			    if(y == x) return 1; // co sinh vien cung ngay sinh
	        }
	}
	return 0; // khong co sinh vien cung ngay sinh
}
node del_cung_ngay_sinh(node first) // ham xoa cac sinh vien cung ngay sinh
{
	if(check(first) == 1)
	{
	    node pi;
	    int a[31] ;
	    int i;
		for(i = 1; i <= 31; i++) a[i] = 1; //khoi tao mang a
	    // a[i] = j: co j sinh vien cung ngay sinh i
	    for(pi = first; pi->link != NULL; pi = pi->link) // duyet list 
		{
			node pj;
		    int x = pi->data.ngaySinh.ngay;
		    for(pj = pi->link; pj != NULL; pj = pj->link)
			{
				int y = pj->data.ngaySinh.ngay;
			    if(y == x) // co sinh vien cung ngay sinh
			    {
				    a[x]++; // so sinh vien cung ngay sinh tang len 
				}
	        }
	    }
	    for( i = 1; i <= 31; i++)
	        //printf("%d: %d\n",i,a[i]);
	    	for(pi = first; pi != NULL; pi = pi->link){
	    		if( a[i] > 1 && i == pi->data.ngaySinh.ngay){ // co hon 2 sinh vien cung ngay sinh
	    			first = delBirthday(first, pi->data.ngaySinh.ngay);// xoa sinh vien cung ngay sinh
				}
			} 
		printf("\n\nDa loai bo cac sinh vien cung ngay sinh\n");
    }
    else printf("\n\nKhong tim thay sinh vien cung ngay sinh\n");
    return first;
}
// Cac Ham input output
sinhvien in() // ham nhap thong tin sinh vien
{
	sinhvien x;
	fflush(stdin);
	printf("\n\tMa Sinh Vien: ");
	gets(x.maSV);
	printf("\tHo va Ten: ");
	gets(x.hoTen);
	printf("\tGioi tinh ( 1:Nam, 0:Nu ): ");
	scanf("%d",&x.gioiTinh);
	printf("\tNgay sinh: ");
	scanf("%d %d %d",&x.ngaySinh.ngay, &x.ngaySinh.thang, &x.ngaySinh.nam);
	fflush(stdin);
	printf("\tDia chi: ");
	gets(x.diaChi);
	printf("\tLop: ");
    gets(x.lop);
	printf("\tKhoa: ");
	gets(x.khoa);
	return x;
}
node input(node first) // ham nhap mot danh sach sinh vien
{ 
	int i, n;
	printf("\n\nSo luong sinh vien: ");
	scanf("%d", &n);
	while( i < n ){
		printf("\n-----THONG TIN SINH VIEN-----\n");
		sinhvien info;
		info = in(); // nhap thong tin sinh vien
		node sv = Create(info); // tao node tu thong tin
		first = addtail(first, sv);// them node vao list
		i++;
	}
	return first;
}
node input_1_sv(node first) // ham them mot sinh vien
{
	printf("\n-----THONG TIN SINH VIEN-----\n");
	sinhvien info;
	info = in();// nhap thong tin sinh vien
	node sv = Create(info); // tao node tu thong tin
	first = addtail(first, sv); // them node vao list
	return first;
}
void output_cung_ngay_sinh(node first) // ham in ra cac sinh vien cung ngay sinh theo ten
{
	if(check(first) == 1)
	{
		printf("\t\nDanh sach sinh vien cung ngay sinh:\n\n");
		node pi;
	    int a[31] ;
	    int i;
		for(i = 1; i <= 31; i++) a[i] = 1; //khoi tao mang a
	    // a[i] = j: co j sinh vien cung ngay sinh i
	    for(pi = first; pi->link != NULL; pi = pi->link)
		{
			node pj;
		    int x = pi->data.ngaySinh.ngay;
		    for(pj = pi->link; pj != NULL; pj = pj->link)
			{
				int y = pj->data.ngaySinh.ngay;
			    if(y == x) // co sinh vien cung ngay sinh
			    {
				    a[x]++;// so sinh vien cung ngay sinh tang len
				}
	        }
	    }
	    for( i = 1; i <= 31; i++){
	    	for(pi = first; pi != NULL; pi = pi->link){
	    		if( a[i] > 1 && i == pi->data.ngaySinh.ngay){ // co hon 2 sinh vien cung ngay sinh
					printf("\t%s\n",pi->data.hoTen);
				}
			}
			if(a[i] > 1) printf("\n"); // chia ra cac nhom sinh vien cung ngay sinh khac nhau
		}
    }
    else printf("\n\nKhong tim thay sinh vien cung ngay sinh\n");
}
void output(node first) // ham in ra danh sach sinh vien theo ten
{ 
    node px;
	printf("\n");
	printf("\nDanh sach sinh vien sap xep theo ma sinh vien: \n\n");
	for(px = first ; px != NULL ; px=px->link){
        printf("\t%s\n",px->data.hoTen);
	    }
	    
}

// Ham xu li tung yeu cau
void yeucau(){ 

	list ListSV;
	ListSV.first = NULL; 
	char c;
	do
	{
		printf("\n\t DANH SACH QUAN LY SINH VIEN:");
	    printf("\n 1 - Them mot danh sach sinh vien");
	    printf("\n 2 - Them mot sinh vien vao danh sach");
	    printf("\n 3 - In danh sach sap xep theo ma sinh vien");
	    printf("\n 4 - Tim kiem sinh vien cung ngay sinh");
	    printf("\n 5 - Loai bo cac sinh vien cung ngay sinh");
	    printf("\n 0 - Thoat");
	    printf("\n Nhap so tuong ung:");
	    c = getch();
	    switch(c){
		    case '1':
		    ListSV.first = input(ListSV.first);
		    break;
		    case '2':
			ListSV.first = input_1_sv(ListSV.first);
			break;	
			case '3':
			if(ListSV.first == NULL) printf("\n\n\tDanh sach dang trong\n");
			else{
			ListSV.first = sort(ListSV.first);
			output(ListSV.first);}
			break;
			case '4':
			if(ListSV.first == NULL) printf("\n\n\tDanh sach dang trong\n");
			else{
			ListSV.first = sort(ListSV.first);
			output_cung_ngay_sinh(ListSV.first);
			}
			break;
			case '5':
			if(ListSV.first == NULL) printf("\n\n\tDanh sach dang trong\n");
			else{
			ListSV.first = del_cung_ngay_sinh(ListSV.first);}
			break;
			
			default:
			printf("\n\n Khong thoa man \n");
			break;
	    }
	} 
	while(c != '0');
	
}
int main() {
	
	yeucau();

	return 0;
}
