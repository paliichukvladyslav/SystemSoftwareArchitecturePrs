from pathlib import Path
from jinja2 import Environment, FileSystemLoader
import shutil

BASE = Path(__file__).parent
OUT = BASE / "output"

# clean
if OUT.exists():
	shutil.rmtree(OUT)
OUT.mkdir()

shutil.copy(BASE / "style.css", OUT / "style.css")
shutil.copytree(BASE / "img", OUT / "img", dirs_exist_ok=True)

env = Environment(loader=FileSystemLoader(BASE / "templates"))

slides = [
    {"file": "01_intro.html", "title": "Вступ", "img": "img/intro.png"},
    {"file": "02_asan_shadowmemory.html", "title": "ASan: принцип роботи (shadow memory)", "img": "img/shadowmemory.png"},
    {"file": "03_asan_redzones.html", "title": "ASan: redzones", "img": "img/redzones.png"},
    {"file": "04_asan_rpi3.html", "title": "ASan: кейс Raspberry Pi 3 (2016)", "img": "img/rpi3bug.png"},
    {"file": "05_asan_building.html", "title": "ASan: компіляція та інструментація", "img": "img/asan_building.png"},
    {"file": "slide05.html", "title": "test", "img": "img/01.png"},
    {"file": "slide05.html", "title": "test", "img": "img/01.png"},
]

slide_tpl = env.get_template("slide.html.j2")
contents_tpl = env.get_template("contents.html.j2")

# generate slides
for i, s in enumerate(slides):
    prev = "index.html" if i == 0 else slides[i-1]["file"]
    next = "index.html" if i == len(slides)-1 else slides[i+1]["file"]

    html = slide_tpl.render(slide=s, prev=prev, next=next)
    (OUT / s["file"]).write_text(html, encoding="utf-8")

# contents
html = contents_tpl.render(slides=slides, title="Лекція 6")
(OUT / "index.html").write_text(html, encoding="utf-8")

print("Done.")
