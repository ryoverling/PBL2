$ErrorActionPreference='Stop'
# Mechanical property updates on the already-refreshed Designer XML files.
$widths=@{
    category_page=@('0,3,1','60,260,140')
    wallet_page=@('0,3,1,2','60,220,130,180')
    transaction_page=@('1,0,1,1,2,1','160,85,100,100,110,140')
    budget_page=@('1,0,1,1,1,1,0','125,60,200,110,110,110,135')
    dashboard_page=@('1,3,1,1,1','160,180,120,120,150')
}
function SetProp($node,$name,$type,$value,$dynamic=$false,$tag='property') {
    $p=$node.SelectSingleNode("$tag[@name='$name']")
    if($p){[void]$node.RemoveChild($p)}
    $p=$node.OwnerDocument.CreateElement($tag);$p.SetAttribute('name',$name)
    if($dynamic){$p.SetAttribute('stdset','0')}
    $v=$node.OwnerDocument.CreateElement($type);$v.InnerText=[string]$value
    [void]$p.AppendChild($v);[void]$node.PrependChild($p)
}
foreach($file in Get-ChildItem (Join-Path $PSScriptRoot ui) -Filter '*.ui') {
    [xml]$doc=Get-Content $file.FullName -Raw -Encoding UTF8
    foreach($table in @($doc.SelectNodes('//widget[@class="QTableWidget"]'))) {
        $spec=$widths[$file.BaseName]
        SetProp $table columnWeights string $spec[0] $true
        SetProp $table columnMinimumWidths string $spec[1] $true
        SetProp $table horizontalHeaderStretchLastSection bool false $false attribute
    }
    foreach($spin in @($doc.SelectNodes('//widget[@class="QDoubleSpinBox"]'))) {
        SetProp $spin showGroupSeparator bool true
    }
    foreach($field in @($doc.SelectNodes('//widget[@class="QComboBox" or @class="QDateEdit"]'))) {
        SetProp $field accessibleName string $field.GetAttribute('name')
    }
    if($file.BaseName -eq 'category_page') {
        SetProp ($doc.SelectSingleNode('//widget[@name="filterCombo"]')) minimumContentsLength number 16
    }
    if($file.BaseName -eq 'budget_dialog') {
        SetProp ($doc.SelectSingleNode('//widget[@name="referenceLabel"]')) text string 'Ngày trong kỳ'
        SetProp ($doc.SelectSingleNode('//widget[@name="referenceDate"]')) toolTip string 'Chọn một ngày bất kỳ trong tuần, tháng hoặc năm muốn lập ngân sách.'
    }
    $settings=[System.Xml.XmlWriterSettings]::new();$settings.Indent=$true;$settings.IndentChars=' ';$settings.Encoding=[System.Text.UTF8Encoding]::new($false)
    $writer=[System.Xml.XmlWriter]::Create($file.FullName,$settings)
    try {$doc.Save($writer)} finally {$writer.Dispose()}
}
