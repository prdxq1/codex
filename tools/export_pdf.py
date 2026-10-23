"""Экспорт готового DOCX в PDF через LibreOffice Writer + Math.
Пример: python tools/export_pdf.py --soffice /path/to/soffice
Сначала выполните python tools/build_report.py с нужными ФИО/группой.
"""
from pathlib import Path
import argparse, subprocess, tempfile, re
root=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser();p.add_argument('--soffice',default='soffice');args=p.parse_args()
with tempfile.TemporaryDirectory(prefix='lab-office-') as profile:
 subprocess.run([args.soffice,'-env:UserInstallation='+Path(profile).as_uri(),'--headless','--convert-to','pdf','--outdir',str(root/'report'),str(root/'report/Лабораторная работа.docx')],check=True)
 path=root/'report/Лабораторная работа.pdf'
 if not path.exists():raise RuntimeError('PDF was not generated')
 # Проверяем, что оглавление согласуется с фактической пагинацией.
 text=subprocess.check_output(['pdftotext','-layout',str(path),'-'],text=True)
 pages=text.split('\f');toc=pages[1]
 for heading in ['Постановка задачи','Математическое описание алгоритма','Алгоритмы выделения контуров','Линейные усредняющие фильтры','Программное описание алгоритма','Результат работы программы','Заключение','Приложение А','Приложение Б']:
  line=next(line for line in toc.splitlines() if heading in line)
  page=int(re.search(r'(\d+)\s*$',line).group(1))
  normalized=re.sub(r'\s+','',pages[page-1])
  if re.sub(r'\s+','',heading) not in normalized:
   raise RuntimeError('Изменилась пагинация: обновите --pages для build_report.py: '+heading)
 if 'Ошибка!' in text:raise RuntimeError('Broken field in report')
 print('PDF generated; contents and page numbers verified')
