$ErrorActionPreference = 'Stop'
$formDir = Join-Path $PSScriptRoot 'ui'
function Prop($node, $name, $type, $value, $tag = 'property') {
    $old = $node.SelectSingleNode("$tag[@name='$name']")
    if ($old) { [void]$node.RemoveChild($old) }
    $p = $node.OwnerDocument.CreateElement($tag)
    $p.SetAttribute('name', $name)
    $v = $node.OwnerDocument.CreateElement($type)
    $v.InnerText = [string]$value
    [void]$p.AppendChild($v)
    [void]$node.PrependChild($p)
}
function Size($node, $name, $w, $h) {
    Prop $node $name size ''
    $s = $node.SelectSingleNode("property[@name='$name']/size")
    foreach ($pair in @(@('width',$w),@('height',$h))) {
        $v = $node.OwnerDocument.CreateElement($pair[0]); $v.InnerText = [string]$pair[1]; [void]$s.AppendChild($v)
    }
}
function Find($name) { $doc.SelectSingleNode("//*[@name='$name']") }
function LabelItem($name, $text, $role = 'muted') {
    $item = $doc.CreateElement('item'); $widget = $doc.CreateElement('widget')
    $widget.SetAttribute('class','QLabel'); $widget.SetAttribute('name',$name)
    Prop $widget text string $text
    Prop $widget wordWrap bool true
    Prop $widget styleRole string $role
    $widget.SelectSingleNode("property[@name='styleRole']").SetAttribute('stdset','0')
    [void]$item.AppendChild($widget)
    return ,$item
}
function LayoutItem($class, $name) {
    $item=$doc.CreateElement('item');$layout=$doc.CreateElement('layout')
    $layout.SetAttribute('class',$class);$layout.SetAttribute('name',$name)
    [void]$item.AppendChild($layout)
    return ,$item
}
function SpacerItem($name) {
    $item=$doc.CreateElement('item');$sp=$doc.CreateElement('spacer');$sp.SetAttribute('name',$name)
    Prop $sp orientation enum 'Qt::Horizontal';Size $sp sizeHint 20 20
    [void]$item.AppendChild($sp);return ,$item
}
$common = @'
QWidget { color: #24344b; font-family: "Segoe UI"; font-size: 13px; }
QLabel { background: transparent; border: none; }
QLabel[styleRole="muted"], QLabel#dateLabel, QLabel#summaryCaption { color: #64748b; }
QLabel#pageTitle, QLabel#formTitle { font-size: 25px; font-weight: 700; color: #172b4d; }
QLabel[styleRole="section"] { font-size: 15px; font-weight: 600; color: #24344b; }
QLabel#statusLabel { color: #64748b; padding: 8px 0; }
QLabel#emptyLabel { color: #64748b; padding: 24px; background: #f8fafc; border-radius: 8px; }
QLabel#errorLabel, QLabel#messageLabel { color: #b42318; }
QFrame[styleRole="card"] { background: white; border: 1px solid #e2e8f0; border-radius: 12px; }
QPushButton { background: white; color: #334155; border: 1px solid #cbd5e1; border-radius: 7px; padding: 8px 16px; min-height: 20px; font-weight: 600; }
QPushButton:hover { background: #f1f5fb; border-color: #94a3b8; }
QPushButton:pressed { background: #e2e8f0; }
QPushButton:focus { border: 1px solid #2563eb; }
QPushButton:disabled { background: #f8fafc; color: #94a3b8; border-color: #e2e8f0; }
QPushButton#addButton, QPushButton#loginButton, QPushButton:default { background: #2563eb; color: white; border-color: #2563eb; }
QPushButton#addButton:hover, QPushButton#loginButton:hover, QPushButton:default:hover { background: #1d4ed8; border-color: #1d4ed8; }
QPushButton#deleteButton:enabled { color: #b42318; border-color: #f2c6c2; }
QPushButton#deleteButton:enabled:hover { background: #fff1f0; }
QLineEdit, QComboBox, QDateEdit, QDoubleSpinBox, QTextEdit { background: white; border: 1px solid #cbd5e1; border-radius: 6px; padding: 7px 10px; min-height: 20px; selection-background-color: #dbeafe; selection-color: #172b4d; }
QComboBox, QDateEdit, QDoubleSpinBox { padding-right: 22px; }
QLineEdit:focus, QComboBox:focus, QDateEdit:focus, QDoubleSpinBox:focus, QTextEdit:focus { border-color: #2563eb; }
QLineEdit:disabled, QComboBox:disabled, QDoubleSpinBox:disabled { color: #64748b; background: #f1f5f9; }
QCheckBox { spacing: 8px; }
QTableWidget { background: white; alternate-background-color: #f8fafc; border: none; selection-background-color: #e8f0ff; selection-color: #173b79; outline: 0; }
QTableWidget::item { padding: 6px; border-bottom: 1px solid #edf1f6; }
QTableWidget::item:selected { background: #e8f0ff; color: #173b79; }
QHeaderView::section { background: #f4f7fb; color: #64748b; padding: 10px 7px; border: none; border-bottom: 1px solid #e2e8f0; font-weight: 600; }
QTableCornerButton::section { background: #f4f7fb; border: none; }
QProgressBar { background: #e8eef8; border: none; border-radius: 6px; min-height: 22px; text-align: center; color: #173b79; }
QProgressBar::chunk { background: #93b9ff; border-radius: 6px; }
QToolTip { color: #24344b; background: white; border: 1px solid #cbd5e1; padding: 5px; }
'@
$subtitles = @{
    dashboard_page='Tài sản, thu chi và các khoản chi trong tháng của bạn.'
    transaction_page='Tìm kiếm, lọc và theo dõi các giao dịch đã ghi nhận.'
    wallet_page='Quản lý nơi lưu tiền và số dư của từng ví.'
    budget_page='Đặt giới hạn chi tiêu theo danh mục và khoảng thời gian.'
    statistics_page='Xem dòng tiền theo kỳ và xu hướng chi tiêu qua các tháng.'
    category_page='Sắp xếp các khoản thu, chi để theo dõi dễ dàng hơn.'
}
$dialogTitles = @{category_dialog='Thông tin danh mục';wallet_dialog='Thông tin ví';transaction_dialog='Thông tin giao dịch';budget_dialog='Thiết lập ngân sách'}
$dialogHelp = @{
    category_dialog='Đặt tên dễ nhận biết và chọn loại thu hoặc chi.'
    wallet_dialog='Số dư ban đầu chỉ được nhập khi tạo ví mới.'
    transaction_dialog='Chọn loại giao dịch, số tiền và ví sử dụng. Thời gian được ghi tự động.'
    budget_dialog='Chọn hạn mức và chu kỳ. Ngân sách chỉ theo dõi chi tiêu, không thay đổi số dư ví.'
}
foreach($file in Get-ChildItem $formDir -Filter '*.ui') {
    [xml]$doc = Get-Content $file.FullName -Raw -Encoding UTF8
    $root=$doc.ui.widget;$base=$file.BaseName;$rootName=$root.GetAttribute('name')
    # Remove the former per-label styling; each Designer form now owns its complete style.
    foreach($p in @($doc.SelectNodes('//widget/property[@name="styleSheet"]'))) { [void]$p.ParentNode.RemoveChild($p) }
    Prop $root styleSheet string ($common + "`nQWidget#$rootName { background: #f5f7fb; }")
    foreach($frame in @($doc.SelectNodes('//widget[@class="QFrame"]'))) {
        if($frame.GetAttribute('name') -match '(?i)card$') {
            Prop $frame styleRole string card;$frame.SelectSingleNode('property[@name="styleRole"]').SetAttribute('stdset','0')
        }
    }
    foreach($label in @($doc.SelectNodes('//widget[@class="QLabel"]'))) {
        $name=$label.GetAttribute('name')
        if($name -match '(Caption|DetailLabel)$') {Prop $label styleRole string muted;$label.SelectSingleNode('property[@name="styleRole"]').SetAttribute('stdset','0')}
        if($name -match '^(chartTitle|budgetTitle|recentTitle|donutTitle|barTitle|sectionTitle)$') {Prop $label styleRole string section;$label.SelectSingleNode('property[@name="styleRole"]').SetAttribute('stdset','0')}
        if($name -match '^(totalBalanceLabel|incomeLabel|expenseLabel|differenceLabel|totalLabel)$') {
            Prop $label styleSheet string 'font-size: 24px; font-weight: 700; color: #172b4d; padding-top: 4px;'
        }
    }
    foreach($table in @($doc.SelectNodes('//widget[@class="QTableWidget"]'))) {
        Prop $table showGrid bool false;Prop $table alternatingRowColors bool false
        Prop $table wordWrap bool false
        Prop $table horizontalHeaderStretchLastSection bool true attribute
        Prop $table horizontalHeaderDefaultSectionSize number 150 attribute
        Prop $table verticalHeaderDefaultSectionSize number 44 attribute
        Prop $table verticalHeaderMinimumSectionSize number 40 attribute
        Prop $table horizontalHeaderHighlightSections bool false attribute
    }
    if($subtitles.ContainsKey($base)) {
        $layout=Find rootLayout
        $header=Find headerLayout
        $title=(Find pageTitle).ParentNode
        $heading=LayoutItem QVBoxLayout headingLayout
        Prop $heading.layout spacing number 4
        [void]$header.InsertBefore($heading,$title)
        [void]$heading.layout.AppendChild($title)
        [void]$heading.layout.AppendChild((LabelItem pageSubtitle $subtitles[$base]))
        $geometry=$root.SelectSingleNode('property[@name="geometry"]')
        if(!$geometry) {
            Prop $root geometry rect ''
            $root.SelectSingleNode('property[@name="geometry"]/rect').InnerXml='<x>0</x><y>0</y><width>1040</width><height>740</height>'
        }
    }
    if($base -in @('category_page','wallet_page','budget_page','transaction_page')) {
        $table=Find table;$cardLayout=$table.ParentNode.ParentNode
        $footer=LayoutItem QHBoxLayout tableFooter
        [void]$footer.layout.AppendChild((LabelItem statusLabel '0 mục • Chọn một dòng để sửa hoặc xóa.'))
        [void]$footer.layout.AppendChild((SpacerItem footerSpacer))
        foreach($name in @('editButton','deleteButton')) {
            $button=Find $name
            Prop $button enabled bool false
            Prop $button toolTip string 'Chọn một dòng trong bảng để thực hiện thao tác.'
            [void]$footer.layout.AppendChild($button.ParentNode)
        }
        [void]$cardLayout.AppendChild($footer)
        $empty=LabelItem emptyLabel 'Chưa có dữ liệu. Nhấn nút thêm ở góc trên để bắt đầu.'
        Prop $empty.widget alignment set 'Qt::AlignCenter'
        [void]$cardLayout.InsertBefore($empty,$table.ParentNode)
        $cardLayout.SetAttribute('stretch', (($cardLayout.SelectNodes('item') | ForEach-Object {if($_.widget -and $_.widget.GetAttribute('name') -eq 'table'){'1'}else{'0'}}) -join ','))
    }
    if($dialogTitles.ContainsKey($base)) {
        $layout=$root.SelectSingleNode('layout')
        foreach($side in @('leftMargin','topMargin','rightMargin','bottomMargin')) {Prop $layout $side number 28}
        Prop $layout spacing number 18
        $firstItem=$layout.SelectSingleNode('item')
        [void]$layout.InsertBefore((LabelItem formTitle $dialogTitles[$base] section),$firstItem)
        [void]$layout.InsertBefore((LabelItem formDescription $dialogHelp[$base]),$firstItem)
        $form=Find formLayout
        Prop $form verticalSpacing number 16;Prop $form horizontalSpacing number 18
        Prop $form labelAlignment set 'Qt::AlignLeft|Qt::AlignVCenter'
        Prop $form fieldGrowthPolicy enum 'QFormLayout::AllNonFixedFieldsGrow'
        Size $root minimumSize 540 0
        Prop $root styleSheet string ($common + "`nQDialog#$rootName { background: white; }")
        # Keyboard labels make the form navigable without a mouse.
        foreach($row in @($form.SelectNodes('item[@column="0"]'))) {
            $field=$form.SelectSingleNode("item[@row='$($row.GetAttribute('row'))'][@column='1']/widget")
            if($field -and $field.GetAttribute('class') -ne 'QWidget' -and $row.widget) {Prop $row.widget buddy cstring $field.GetAttribute('name')}
        }
    }
    if($base -eq 'main_window') {
        Size (Find sidebar) minimumSize 212 0;Size (Find sidebar) maximumSize 212 16777215
        Prop (Find brandLabel) text string 'MoneyManager'
        Prop (Find userLabel) wordWrap bool true
        $sidebar=Find sidebarLayout
        $navItem=(Find dashboardButton).ParentNode
        [void]$sidebar.InsertBefore((LabelItem navigationCaption 'KHÔNG GIAN CÁ NHÂN'),$navItem)
        $logout=(Find logoutButton).ParentNode
        [void]$sidebar.InsertBefore((LabelItem storageCaption "Dữ liệu lưu trên máy`nQuản lý tài chính cá nhân"),$logout)
        $sideStyle=@'
QMainWindow, QWidget#centralWidget, QStackedWidget#pageStack { background: #f5f7fb; }
QFrame#sidebar { background: white; border: none; border-right: 1px solid #e2e8f0; }
QFrame#sidebar QLabel#brandLabel { color: #1d4ed8; font-size: 21px; font-weight: 700; padding: 8px 0; }
QFrame#sidebar QLabel#userLabel { color: #64748b; font-size: 12px; }
QLabel#navigationCaption { color: #94a3b8; font-size: 10px; font-weight: 600; padding-bottom: 8px; }
QLabel#storageCaption { color: #94a3b8; font-size: 11px; padding: 10px 0; }
QFrame#sidebar QPushButton { text-align: left; border: none; background: transparent; padding: 12px 14px; color: #64748b; font-weight: 500; }
QFrame#sidebar QPushButton:hover { background: #f1f5f9; }
QFrame#sidebar QPushButton:checked { background: #eaf1ff; color: #1d4ed8; font-weight: 700; }
QFrame#sidebar QPushButton:focus { border: 1px solid #93b9ff; }
QFrame#sidebar QPushButton#logoutButton { border-top: 1px solid #e2e8f0; border-radius: 0; }
'@
        Prop $root styleSheet string ($common+"`n"+$sideStyle)
    }
    if($base -eq 'statistics_page') {
        $header=Find headerLayout;$filters=LayoutItem QHBoxLayout periodControls
        [void]$filters.layout.AppendChild((LabelItem periodCaption 'Kỳ thống kê'))
        foreach($name in @('periodCombo','referenceDate','customStart','customEnd')) {
            [void]$filters.layout.AppendChild((Find $name).ParentNode)
            if($name -eq 'referenceDate') {
                [void]$filters.layout.AppendChild((LabelItem customFromLabel 'Từ'))
            }
            if($name -eq 'customStart') {[void]$filters.layout.AppendChild((LabelItem customToLabel 'Đến'))}
        }
        [void]$filters.layout.AppendChild((SpacerItem periodSpacer))
        [void](Find rootLayout).InsertAfter($filters,$header.ParentNode)
        [void](Find rootLayout).InsertAfter((LabelItem rangeLabel 'Khoảng thời gian được chọn'),$filters)
    }
    if($base -eq 'transaction_page') {
        $row=Find filterRow2
        [void]$row.InsertBefore((LabelItem periodCaption 'Thời gian'),$row.SelectSingleNode('item'))
        [void]$row.InsertBefore((LabelItem fromLabel 'Từ'),(Find fromDate).ParentNode)
        [void]$row.InsertBefore((LabelItem toLabel 'Đến'),(Find toDate).ParentNode)
        Prop (Find searchInput) placeholderText string 'Tìm ghi chú, danh mục hoặc ví…'
        [void](Find cardLayout).InsertAfter((LabelItem errorLabel ''),(Find filterRow2).ParentNode)
    }
    if($base -eq 'dashboard_page') {
        Prop (Find recentTitle) text string 'Các khoản chi tháng này'
        $table=Find recentTable
        $cols=$table.SelectNodes('column')
        Prop $cols[1] text string 'Khoản chi'
        Prop (Find chartTitle) text string 'Phân bổ chi tiêu'
        [void](Find recentCard).SelectSingleNode('layout').AppendChild((LabelItem expenseHint 'Chi tiết theo từng giao dịch • Tên khoản chi và số tiền'))
        [void](Find budgetCard).SelectSingleNode('layout').AppendChild((LabelItem budgetHint 'Các ngân sách có thể trùng phạm vi; tổng theo dõi không phải số dư khả dụng.'))
    }
    if($base -eq 'login_window') {
        Prop (Find brandLabel) text string 'MoneyManager'
        Prop (Find sloganLabel) text string "Hiểu dòng tiền.`nChủ động chi tiêu.`n`nGhi nhận thu chi, quản lý ví và`ntheo dõi ngân sách trong một nơi."
        Prop (Find hintLabel) wordWrap bool true
        Prop (Find titleLabel) styleSheet string 'font-size: 28px; font-weight: 700; color: #172b4d;'
        Prop (Find brandPanel) styleSheet string 'QFrame#brandPanel { background: #eaf1ff; border: none; } QLabel { color: #1e40af; background: transparent; } QLabel#brandLabel { font-size: 28px; font-weight: 700; } QLabel#sloganLabel { font-size: 17px; }'
        Prop (Find formPanel) styleSheet string 'QFrame#formPanel { background: white; border: none; }'
        Size $root minimumSize 900 580
        Prop (Find hintLabel) styleSheet string 'color: #64748b; font-size: 12px;'
    }
    $settings=[System.Xml.XmlWriterSettings]::new();$settings.Indent=$true;$settings.IndentChars=' ';$settings.Encoding=[System.Text.UTF8Encoding]::new($false)
    $writer=[System.Xml.XmlWriter]::Create($file.FullName,$settings)
    try {$doc.Save($writer)} finally {$writer.Dispose()}
}
