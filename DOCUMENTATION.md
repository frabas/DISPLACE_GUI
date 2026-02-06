# DISPLACE Documentation Guide

## 📚 Documentation Overview

DISPLACE documentation is built using [Quarto](https://quarto.org/), an open-source scientific publishing system that supports reproducible documentation with integrated code, visualizations, and narrative text.

### Where to Find Documentation

- **Main Documentation**: See the `docs/` directory
- **Online Documentation**: *(Coming soon - will be published to GitHub Pages)*
- **Quick Reference**: See `docs/QUICKREF.md` for common tasks
- **Contributing Guide**: See `docs/CONTRIBUTING_DOCS.md`

## 🚀 Quick Start for Contributors

### 1. Install Prerequisites

- **Quarto**: Download from [quarto.org](https://quarto.org/docs/get-started/)
- **Optional - R**: For R code examples ([r-project.org](https://www.r-project.org/))

### 2. Preview Documentation Locally

```bash
cd docs
quarto preview
```

This opens a live preview in your browser that auto-updates as you edit.

### 3. Add New Content

**Easy Way** - Use the template:
```bash
cd docs
cp _template_chapter.qmd my-new-topic.qmd
# Edit my-new-topic.qmd
# Add to _quarto.yml
quarto preview
```

**Step-by-Step Guide**: See `docs/README.md`

## 📖 Documentation Structure

The documentation is organized as a book with the following parts:

### Getting Started
- Quick Start - Installation and setup
- Building - Compilation instructions  
- Running - How to run DISPLACE

### User Guide
- R Routines - Data preparation workflows
- Output Formats - Understanding simulation outputs

### Advanced Topics
*(Coming soon - add your chapters here!)*

## ✏️ How to Contribute

### Adding New Documentation

1. **Copy the template**:
   ```bash
   cd docs
   cp _template_chapter.qmd your-topic.qmd
   ```

2. **Edit your chapter** with content

3. **Add to `_quarto.yml`**:
   ```yaml
   book:
     chapters:
       - part: "Section Name"
         chapters:
           - your-topic.qmd
   ```

4. **Preview and test**:
   ```bash
   quarto preview
   ```

5. **Commit and push**:
   ```bash
   git add docs/
   git commit -m "docs: add chapter on [topic]"
   git push
   ```

### Updating Existing Documentation

1. Edit the relevant `.qmd` file in `docs/`
2. Preview your changes with `quarto preview`
3. Commit and push

For detailed guidelines, see:
- **`docs/README.md`** - Comprehensive documentation guide
- **`docs/CONTRIBUTING_DOCS.md`** - Detailed contribution guidelines
- **`docs/QUICKREF.md`** - Quick reference for common tasks

## 🎯 Key Features

### Easy to Expand
- **Template provided** - Just copy and fill in
- **Clear structure** - Organized by topic
- **Live preview** - See changes immediately
- **Markdown-based** - No complex tools needed

### Integrated Code
- Execute R, Python, Julia, etc.
- Show code and output together
- Reproducible examples

### Professional Output
- Clean HTML with navigation
- Automatic table of contents
- Search functionality
- Cross-references between chapters
- Bibliography support

### Version Controlled
- All in Git
- Track changes over time
- Collaborative editing

## 📁 File Organization

```
DISPLACE_GUI/
├── docs/                          # Main documentation directory
│   ├── _quarto.yml               # Documentation configuration
│   ├── README.md                  # Documentation guide
│   ├── CONTRIBUTING_DOCS.md       # How to contribute
│   ├── QUICKREF.md                # Quick reference
│   ├── _template_chapter.qmd      # Template for new chapters
│   │
│   ├── index.qmd                  # Homepage
│   ├── quickstart.qmd             # Getting started
│   ├── building.qmd               # Build instructions
│   ├── running.qmd                # Running DISPLACE
│   ├── routines.qmd               # R routines
│   ├── output-formats.qmd         # Output formats
│   │
│   ├── references.bib             # Bibliography
│   └── images/                    # Documentation images
│
└── quarto_docs/                   # Legacy tutorial (being migrated)
    └── quarto-tutorial.qmd
```

## 🔗 Useful Links

- [DISPLACE Project Website](https://displace-project.org/)
- [Quarto Documentation](https://quarto.org/docs/guide/)
- [Quarto Books Guide](https://quarto.org/docs/books/)
- [Markdown Guide](https://www.markdownguide.org/)

## 💬 Getting Help

- **Documentation Questions**: See `docs/README.md`
- **Technical Issues**: Open an issue on GitHub
- **Quarto Help**: [Quarto documentation](https://quarto.org/docs/guide/)
- **General Contributing**: See `CONTRIBUTING.md`

## 🎉 What's New

The documentation system has been redesigned to make it easier to expand and maintain:

✅ **Organized structure** with clear sections
✅ **Template provided** for quick chapter creation
✅ **Comprehensive guides** for contributors
✅ **Live preview** for immediate feedback
✅ **Professional output** with navigation and search
✅ **All existing docs** converted and integrated

## 🚀 Next Steps

1. **Explore** the documentation in `docs/`
2. **Preview** it locally with `quarto preview`
3. **Add** your own chapters using the template
4. **Share** your knowledge with the community!

---

For the legacy README content, see the documentation in `docs/` or visit the [DISPLACE website](https://displace-project.org/).
