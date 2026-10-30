"""Экспорт через LibreOffice Writer + Math, обновление номеров оглавления."""
from pathlib import Path
import argparse, subprocess, tempfile,re
from docx import Document
root=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser();p.add_argument('--soffice',default='soffice');args=p.parse_args()
path=root/'report/Лабораторная работа.docx'
def normalized(s):return re.sub(r'\s+','',s)
with tempfile.TemporaryDirectory(prefix='lab-office-') as profile:
 def render():
  subprocess.run([args.soffice,'-env:UserInstallation='+Path(profile).as_uri(),'--headless','--convert-to','pdf','--outdir',str(root/'report'),str(path)],check=True)
  if not path.with_suffix('.pdf').exists():raise RuntimeError('PDF missing')
  return subprocess.check_output(['pdftotext','-layout',str(path.with_suffix('.pdf')),'-'],text=True).split('\f')
 for attempt in range(3):
  pages=render();d=Document(path);changed=False
  from docx.oxml.ns import qn
  from docx.text.paragraph import Paragraph
  for sdt in d._element.body:
   if sdt.tag!=qn('w:sdt'):continue
   for node in sdt.findall('.//'+qn('w:p')):
    para=Paragraph(node,d._body)
    if '\t' not in para.text:continue
    title=para.text.rsplit('\t',1)[0]
    matches=[i for i,text in enumerate(pages,1) if i>2 and normalized(title) in normalized(text)]
    if not matches:raise RuntimeError('Heading not found: '+title)
    replacement=title+'\t'+str(matches[0])
    if para.text!=replacement:
     para.runs[0].text=replacement
     for run in para.runs[1:]:run.text=''
     changed=True
  if not changed:break
  d.save(path)
 else:raise RuntimeError('Contents did not stabilize')
 assert not any('Ошибка!' in text for text in pages)
 print('PDF and contents verified:',len([x for x in pages if x.strip()]),'pages')
