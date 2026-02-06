# Quick Start: Contributing Documentation

**Want to add documentation to DISPLACE? This 5-minute guide gets you started.**

## ⚡ Super Quick Start (30 seconds)

```bash
# 1. Install Quarto
# Download from: https://quarto.org/docs/get-started/

# 2. Clone and navigate
git clone https://github.com/frabas/DISPLACE_GUI.git
cd DISPLACE_GUI/docs

# 3. Preview
quarto preview

# Done! Your browser shows the documentation.
```

## 📝 Add Your First Chapter (2 minutes)

```bash
# 1. Copy the template
cd docs
cp _template_chapter.qmd my-topic.qmd

# 2. Edit my-topic.qmd with your content
# (Use your favorite text editor)

# 3. Add to _quarto.yml (add this line under a part's chapters):
#    - my-topic.qmd

# 4. Preview to see it live
quarto preview
```

## 🎯 What You Can Add

- **Tutorials**: Step-by-step guides
- **How-tos**: Task-focused instructions  
- **Explanations**: Conceptual information
- **Reference**: Technical specifications
- **Examples**: Code with outputs
- **FAQs**: Common questions

## 📚 File Structure

```
docs/
├── _quarto.yml           ← Add your chapter here
├── _template_chapter.qmd ← Copy this to start
├── your-topic.qmd        ← Your new chapter
├── images/               ← Put images here
└── README.md             ← Full guide (read later)
```

## ✍️ Basic Syntax

**Heading with ID** (for cross-references):
```markdown
## My Section {#sec-my-id}
```

**Code block**:
````markdown
```bash
cd /path
./command
```
````

**Image**:
```markdown
![Caption](images/my-image.png)
```

**Link to another section**:
```markdown
See @sec-my-id for details.
```

**Callout box**:
```markdown
::: {.callout-tip}
Helpful tip here!
:::
```

## 🔄 Workflow

```bash
# Edit files
# ↓
# Save
# ↓  
# Browser auto-refreshes
# ↓
# Repeat until happy
# ↓
git add docs/
git commit -m "docs: add chapter on X"
git push
```

## 📖 Need More Help?

- **Quick reference**: `docs/QUICKREF.md` (all syntax)
- **Full guide**: `docs/README.md` (everything)
- **Contribute guide**: `docs/CONTRIBUTING_DOCS.md` (detailed workflow)
- **Summary**: `docs/IMPLEMENTATION_SUMMARY.md` (what was done)

## 💡 Pro Tips

1. **Preview often** - Catch errors early
2. **Use the template** - Don't start from scratch
3. **Keep it simple** - Markdown is enough
4. **Add images** - Stored in `images/` directory
5. **Cross-reference** - Link chapters together

## ❓ Common Questions

**Q: I don't have Quarto installed**  
A: Get it from https://quarto.org/docs/get-started/ (5-minute install)

**Q: What if I just want to edit text?**  
A: Edit any `.qmd` file, save, preview updates automatically

**Q: Can I write in regular Markdown?**  
A: Yes! `.qmd` files are Markdown with optional extras

**Q: Do I need to know R/Python?**  
A: No, unless you want to include code examples

**Q: How do I add my chapter to the book?**  
A: Edit `_quarto.yml`, add your filename under `chapters:`

**Q: Preview not working?**  
A: Save the file, or restart with `Ctrl+C` then `quarto preview`

## 🎉 You're Ready!

That's it! You now know enough to contribute documentation. 

**Start with**:
```bash
cd docs
cp _template_chapter.qmd my-first-chapter.qmd
# Edit it
quarto preview
```

**For more details, read**:
- `README.md` - Complete guide
- `QUICKREF.md` - Syntax reference  
- `CONTRIBUTING_DOCS.md` - Full workflow

Happy documenting! 📚✨
