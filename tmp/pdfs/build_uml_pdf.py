from pathlib import Path
import math
from reportlab.pdfgen import canvas
from reportlab.lib.colors import HexColor, white
from reportlab.pdfbase import pdfmetrics
from reportlab.pdfbase.ttfonts import TTFont
from reportlab.lib.pagesizes import A4, landscape

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / 'output/pdf/MoneyManager_UML.pdf'
OUT.parent.mkdir(parents=True, exist_ok=True)
pdfmetrics.registerFont(TTFont('Arial', 'C:/Windows/Fonts/arial.ttf'))
pdfmetrics.registerFont(TTFont('ArialBold', 'C:/Windows/Fonts/arialbd.ttf'))
W, H = landscape(A4)
c = canvas.Canvas(str(OUT), pagesize=(W, H))
c.setTitle('MoneyManager - UML và kiến trúc ứng dụng')
c.setAuthor('MoneyManager - PBL2')
INK = '#20334A'
BLUE = '#246BA0'
PALE = '#EDF5FB'
GRAY = '#526477'

def text(x, y, value, size=10, bold=False, color=INK, center=False):
    c.setFillColor(HexColor(color))
    c.setFont('ArialBold' if bold else 'Arial', size)
    (c.drawCentredString if center else c.drawString)(x, H-y, value)

def rect(x, y, w, h, fill='FFFFFF', stroke='BDD0DF'):
    c.setFillColor(HexColor('#'+fill.lstrip('#')))
    c.setStrokeColor(HexColor('#'+stroke.lstrip('#')))
    c.setLineWidth(.8)
    c.roundRect(x, H-y-h, w, h, 5, fill=1, stroke=1)

def line(points, color=GRAY, dashed=False, width=1):
    c.setStrokeColor(HexColor(color)); c.setLineWidth(width)
    c.setDash(3, 3) if dashed else c.setDash()
    p = c.beginPath(); p.moveTo(points[0][0], H-points[0][1])
    for x,y in points[1:]: p.lineTo(x,H-y)
    c.drawPath(p); c.setDash()

def arrow(points, label=None, lx=None, ly=None, inheritance=False, dashed=False):
    line(points, dashed=dashed)
    (ax,ay),(bx,by)=points[-2:]
    dx,dy=bx-ax,by-ay; n=math.hypot(dx,dy); dx/=n; dy/=n
    length=11 if inheritance else 6; wide=5 if inheritance else 3
    p=c.beginPath(); p.moveTo(bx,H-by)
    p.lineTo(bx-dx*length-dy*wide,H-(by-dy*length+dx*wide))
    p.lineTo(bx-dx*length+dy*wide,H-(by-dy*length-dx*wide)); p.close()
    c.setFillColor(white if inheritance else HexColor(GRAY)); c.drawPath(p,fill=1,stroke=1)
    if label: text(lx,ly,label,9,color=GRAY)

def diamond(x,y):
    c.setFillColor(HexColor(GRAY)); c.setStrokeColor(HexColor(GRAY))
    p=c.beginPath(); p.moveTo(x,H-y); p.lineTo(x+4,H-y-6); p.lineTo(x,H-y-12); p.lineTo(x-4,H-y-6); p.close(); c.drawPath(p,fill=1,stroke=1)

def box(x,y,w,title,rows,abstract=False):
    head=43 if abstract else 30
    h=head+len(rows)*16+12
    rect(x,y,w,h,PALE)
    if abstract:
        text(x+w/2,y+15,'«abstract»',9,color=GRAY,center=True)
        text(x+w/2,y+32,title,12,True,center=True)
    else: text(x+w/2,y+20,title,12,True,center=True)
    line([(x,y+head),(x+w,y+head)],'#BDD0DF')
    for i,s in enumerate(rows): text(x+10,y+head+17+i*16,s,10)
    return h

def node(x,y,w,h,title,lines=()):
    rect(x,y,w,h,PALE)
    text(x+w/2,y+22,title,12,True,center=True)
    for i,s in enumerate(lines): text(x+w/2,y+42+15*i,s,10,center=True)

def page(number,title,subtitle):
    c.setFillColor(HexColor(BLUE)); c.rect(0,H-8,W,8,fill=1,stroke=0)
    text(34,35,'MONEYMANAGER  /  PBL2',10,True,color=BLUE)
    text(34,66,title,21,True)
    text(34,88,subtitle,10,color=GRAY)
    line([(34,560),(W-34,560)],'#D7E1EA')
    text(34,579,'C++17  •  Qt 6 Widgets  •  Sơ đồ rút gọn theo mã nguồn dự án',9,color=GRAY)
    text(W-40,579,f'{number} / 4',9,color=GRAY)

page(1,'01. UML các lớp dữ liệu chính','Quan hệ kế thừa và liên kết dữ liệu giữa giao dịch, ví, danh mục và ngân sách.')
box(302,115,238,'Transaction',[
    '- id: int','- amount: double','- dateTime, note: string','- walletId, categoryId: int',
    '+ getType(): TransactionType','+ getSignedAmount(): double'],True)
box(36,125,212,'Income',['+ getType(): TransactionType','+ getSignedAmount(): double'])
box(594,125,212,'Expense',['+ getType(): TransactionType','+ getSignedAmount(): double'])
arrow([(248,167),(302,167)],inheritance=True)
arrow([(594,167),(540,167)],inheritance=True)
box(36,292,212,'Wallet',['- id: int; name: string','- type: WalletType','- balance: double','+ deposit(amount): bool','+ withdraw(amount): bool'])
box(594,292,212,'Category',['- id: int; name: string','- type: CategoryType','+ getType(): CategoryType','+ setName(name): void'])
box(302,362,238,'Budget',['- id: int; amount: double','- startDate, endDate: string','- categoryId: int','+ setAmount(amount): void','+ setDateRange(start, end): void'])
arrow([(302,252),(272,252),(272,325),(248,325)])
text(256,245,'0..*',9); text(251,342,'1',9); text(167,271,'walletId',9,color=GRAY)
arrow([(540,252),(567,252),(567,325),(594,325)])
text(544,245,'0..*',9); text(576,342,'1',9); text(599,271,'categoryId',9,color=GRAY)
arrow([(540,423),(699,423),(699,398)])
text(546,417,'0..*',9); text(708,413,'0..1',9); text(588,444,'categoryId',9,color=GRAY)
text(36,514,'Ký hiệu: - private; + public; tam giác rỗng: kế thừa. Các liên kết được lưu bằng ID.',10)
text(36,531,'Budget.categoryId = 0: áp dụng cho toàn bộ danh mục chi. Các hàm trong lớp Transaction là thuần ảo.',10)
text(36,548,'Lớp User riêng cho đăng nhập: username, password, name, loggedIn; login(), logout(), isLoggedIn().',10)
c.showPage()

page(2,'02. Kiến trúc tổng thể của app','Chia theo trách nhiệm: giao diện, nghiệp vụ, dữ liệu trong bộ nhớ và lưu trữ cục bộ.')
node(307,114,228,70,'MainWindow',['Điều phối ứng dụng','loadData(), saveAll(), refreshAll()'])
node(36,225,212,85,'Qt Widgets UI',['Dashboard, Transaction, Wallet','Category, Budget, Statistics','Tiếp nhận thao tác người dùng'])
node(307,225,228,85,'Các Manager',['Wallet, Category','Transaction, Budget','Kiểm tra và xử lý nghiệp vụ'])
node(594,225,212,85,'FileManager',['Đọc / ghi dữ liệu','Trao đổi dữ liệu với Manager','Kiểm tra kết quả lưu / tải'])
arrow([(307,149),(142,149),(142,225)])
arrow([(421,184),(421,225)])
arrow([(535,149),(700,149),(700,225)])
arrow([(248,265),(307,265)])
arrow([(594,265),(535,265)])
node(36,375,212,72,'Model',['Wallet, Category, Transaction','Income, Expense, Budget'])
node(307,375,228,72,'LinkedList<T> / Node<T>',['Các danh sách trong RAM','Cấu trúc dữ liệu tự cài'])
node(594,375,212,87,'File cục bộ',['wallets.txt, categories.txt','transactions.txt, budgets.txt','Trong thư mục data/'])
arrow([(365,310),(365,342),(142,342),(142,375)])
arrow([(421,310),(421,375)])
arrow([(700,310),(700,375)])
rect(36,484,770,65,'F5F8FB')
text(49,503,'Khởi động: main.cpp tạo QApplication, hiển thị LoginWindow rồi chạy vòng lặp sự kiện app.exec().',10)
text(49,521,'DateUtils hỗ trợ kiểm tra ngày và lọc theo khoảng thời gian. User và user.txt phục vụ đăng nhập local.',10)
text(49,539,'CMake tách MoneyManagerCore, MoneyManagerUi và chương trình MoneyManager; UI liên kết Qt6::Widgets.',10)
c.showPage()

page(3,'03. UML phần quản lý và danh sách liên kết','Mỗi Manager sở hữu một danh sách riêng; kiểu phần tử được xác định bằng template.')
node(315,109,212,48,'MainWindow')
line([(421,157),(421,180)])
diamond(421,157)
line([(124,180),(718,180)])
xs=[36,234,432,630]
names=['WalletManager','CategoryManager','TransactionManager','BudgetManager']
types=['Wallet','Category','Transaction*','Budget']
for x,name,typ in zip(xs,names,types):
    line([(x+88,180),(x+88,204)])
    box(x,204,176,name,[f'- LinkedList<{typ}>'])
    diamond(x+88,262)
    line([(x+88,274),(x+88,306)])
line([(124,306),(718,306)])
line([(421,306),(421,335)])
box(236,335,330,'LinkedList<T>',[
    '- head, tail: Node<T>*; size: size_t',
    '+ pushBack(value), findIf(predicate)',
    '+ removeIf(predicate), forEach(func)',
    '+ sort(compare), clear()'])
box(627,350,179,'Node<T>',['+ data: T','+ next: Node<T>*'])
line([(566,386),(627,386)])
# Horizontal filled diamond at the owning list.
c.setFillColor(HexColor(GRAY)); p=c.beginPath(); p.moveTo(566,H-386); p.lineTo(572,H-382); p.lineTo(578,H-386); p.lineTo(572,H-390); p.close(); c.drawPath(p,fill=1,stroke=1)
text(585,376,'0..*',9)
rect(36,335,178,122,'F5F8FB')
text(47,356,'Ký hiệu và sở hữu',11,True)
text(47,378,'Hình thoi đặc: thành phần',9)
text(47,393,'thuộc sở hữu của lớp chứa.',9)
text(47,415,'LinkedList xóa các node.',9)
text(47,432,'TransactionManager xóa',9)
text(47,447,'các đối tượng giao dịch.',9)
text(36,487,'Phụ thuộc nghiệp vụ:',11,True)
text(36,506,'• TransactionManager dùng WalletManager để cập nhật số dư và CategoryManager để kiểm tra danh mục.',10)
text(36,524,'• BudgetManager dùng TransactionManager để tính tổng chi, số tiền còn lại và kiểm tra vượt ngân sách.',10)
text(36,542,'LinkedList<T> là mẫu lớp dùng chung; các đường nối không biểu thị một danh sách được dùng chung.',10,color=GRAY)
c.showPage()

page(4,'04. UML trình tự: thêm một giao dịch','Luồng thành công và cách xử lý khi không thể thêm giao dịch.')
centers=[64,166,268,370,472,574,676,778]
labels=[['Người dùng'],['Transaction','Page'],['Transaction','Dialog'],['Transaction','Manager'],['Wallet'],['LinkedList'],['MainWindow'],['FileManager']]
for x,ls in zip(centers,labels):
    rect(x-45,110,90,42,PALE)
    for i,s in enumerate(ls): text(x,128+i*13,s,9,True,center=True)
    line([(x,152),(x,491)],'#A8B6C5',True)
def seq(a,b,y,label,dashed=False):
    arrow([(centers[a],y),(centers[b],y)],dashed=dashed)
    text((centers[a]+centers[b])/2,y-5,label,8.5,center=True)
seq(0,1,176,'Bấm Thêm')
seq(1,2,202,'Mở hộp thoại')
seq(0,2,229,'Nhập dữ liệu và xác nhận')
seq(2,1,256,'Income / Expense',True)
seq(1,3,283,'addTransaction(...)')
arrow([(370,297),(425,297),(425,315),(370,315)])
text(444,303,'Kiểm tra ID, số tiền, ngày, ví, danh mục',9)
seq(3,4,340,'deposit / withdraw')
seq(3,5,369,'pushBack(transaction)')
seq(3,1,398,'true',True)
seq(1,6,426,'onChanged()')
seq(6,7,454,'Lưu qua FileManager')
seq(7,6,480,'Kết quả lưu',True)
rect(36,501,770,48,'F5F8FB')
text(48,519,'Sau khi lưu: MainWindow gọi refreshAll(). Nếu lưu thất bại, ứng dụng báo lỗi và giữ cờ chưa lưu.',10)
text(48,537,'Nếu addTransaction() trả false: TransactionPage xóa đối tượng vừa tạo và hiển thị thông báo lỗi.',10)
c.showPage()
c.save()
print(OUT)
