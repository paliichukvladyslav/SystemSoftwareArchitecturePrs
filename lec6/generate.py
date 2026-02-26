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
    {"file": "slide01.html", "title": "test", "img": "img/01.png"},
    {"file": "slide02.html", "title": "test", "img": "img/01.png"},
    {"file": "slide03.html", "title": "test", "img": "img/01.png"},
    {"file": "slide04.html", "title": "test", "img": "img/01.png"},
    {"file": "slide05.html", "title": "test", "img": "img/01.png"},
]

slide_tpl = env.get_template("slide.html.j2")
contents_tpl = env.get_template("contents.html.j2")

# generate slides
for i, s in enumerate(slides):
    prev = "contents.html.j2" if i == 0 else slides[i-1]["file"]
    next = "contents.html.j2" if i == len(slides)-1 else slides[i+1]["file"]

    html = slide_tpl.render(slide=s, prev=prev, next=next)
    (OUT / s["file"]).write_text(html, encoding="utf-8")

# contents
html = contents_tpl.render(slides=slides, title="Лекція 6")
(OUT / "index.html").write_text(html, encoding="utf-8")

print("Done.")
