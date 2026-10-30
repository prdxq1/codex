"""Дополнение отчёта третьим вариантом; оформление наследуется от шаблона."""
from copy import deepcopy
from docx.text.paragraph import Paragraph
from docx.shared import Cm,Pt
from docx.oxml import OxmlElement
from pathlib import Path
import csv

def extend(d,root):
 def before(anchor,text,style='Normal'):
  p=anchor.insert_paragraph_before(text,style);return p
 ps=d.paragraphs
 task=ps[22]
 before(task,'Вариант № 3. Исследовать порядковые фильтры на изображении с гауссовым шумом: медиану, минимум и максимум в локальном окне.')
 anchor=next(p for p in d.paragraphs if p.text=='Программное описание алгоритма')
 p=before(anchor,'2.3. Порядковые фильтры','Heading 2')
 sample=next(q for q in d.paragraphs if q.text=='Линейные усредняющие фильтры')
 p._p.remove(p._p.pPr);p._p.insert(0,deepcopy(sample._p.pPr))
 for num in p._p.xpath('./w:pPr/w:numPr'):num.getparent().remove(num)
 if sample.runs and sample.runs[0]._r.rPr is not None:p.runs[0]._r.insert(0,deepcopy(sample.runs[0]._r.rPr))
 p.paragraph_format.page_break_before=True
 anchor.paragraph_format.page_break_before=True
 for text in [
 'В отличие от линейного усреднения, ранговая обработка выбирает одно из значений окрестности. Сформируем выборку из k² яркостей и мысленно упорядочим её: v₀ ≤ v₁ ≤ … ≤ vₖ²₋₁. Окно нечётное, поэтому центральный элемент определён однозначно.',
 'Медианный фильтр: I′(x,y) = v[k²/2], где индекс вычисляется целочисленным делением. Например, для 3×3 выбирается пятый элемент из девяти. Медиана подавляет отдельные выбросы; при гауссовом шуме она не обязана превосходить линейное усреднение по MSE.',
 'Фильтр минимума: I′(x,y) = min Ω(x,y). Он расширяет тёмные области и сужает светлые. Фильтр максимума: I′(x,y) = max Ω(x,y); его действие противоположно. На полутоновом изображении эти операции соответствуют эрозии и дилатации с квадратным структурным элементом.',
 'Алгоритм 6. Для каждого выходного пикселя собрать окно k×k с репликацией границ, выбрать медиану либо экстремум и записать его в отдельный растр. Для цветного входа каналы обрабатываются независимо. Медиана находится через std::nth_element без полной сортировки; min_element и max_element выбирают крайние значения.']:
  before(anchor,text)
 anchor=next(p for p in d.paragraphs if p.text=='Результат работы программы')
 anchor.paragraph_format.page_break_before=True
 before(anchor,'Третий вариант реализован функциями applyMedianFilter, applyMinFilter и applyMaxFilter. Они вызывают общий rankFilter с режимом 0, −1 или +1. Для каждой позиции вектор values очищается и заполняется яркостями соседей через getPixelChannel, где координаты ограничиваются clampCoord. Новых методов Image не добавлено. Полный листинг приведён в приложении В.')
 anchor=next(p for p in d.paragraphs if p.text=='Заключение')
 before(anchor,'Результаты порядковой обработки показывают различие целей фильтров. Медиана сглаживает случайные выбросы, сохраняя часть резких переходов. Минимум и максимум систематически смещают яркость вниз и вверх, поэтому на гауссовом шуме они не являются универсальным способом восстановления оригинала. Их полезность связана с выбором тёмных или светлых деталей.')
 rows=list(csv.DictReader((root/'test_output/metrics.csv').open()))
 labels={'median3':'медиана 3×3','median5':'медиана 5×5','minimum':'минимум 3×3','maximum':'максимум 3×3'}
 for row in rows:
  if row['method'] in labels:
   before(anchor,f"{labels[row['method']]}: MSE = {float(row['mse']):.2f}; PSNR = {float(row['psnr_db']):.2f} дБ; ошибка ровного фона = {float(row['flat_mse']):.2f}.")
 for i,(name,label) in enumerate([('var3_median_3x3','Медианный фильтр 3×3'),('var3_median_5x5','Медианный фильтр 5×5'),('var3_min','Фильтр минимума 3×3'),('var3_max','Фильтр максимума 3×3')],8):
  p=before(anchor,'');p.paragraph_format.keep_with_next=True;p.alignment=1
  if i in (8,10):p.paragraph_format.page_break_before=True
  p.add_run().add_picture(str(root/'test_output'/f'{name}.png'),width=Cm(6))
  p=before(anchor,f'Рис. {i}. {label}');p.alignment=1

 anchor=next(p for p in d.paragraphs if p.text=='Приложение А')
 p=d.add_paragraph('Приложение В','Heading 1');p.paragraph_format.page_break_before=True
 d.add_paragraph('Реализация порядковых фильтров и общего обхода окна:')
 source=(root/'src/Filters.cpp').read_text().split('    // Вариант 3.')[1].split('} // namespace Filters')[0]
 for line in source.splitlines()[1:]:
  p=d.add_paragraph();p.paragraph_format.space_after=Pt(0);p.paragraph_format.line_spacing=1
  run=p.add_run(line);run.font.name='Consolas';run.font.size=Pt(10)
