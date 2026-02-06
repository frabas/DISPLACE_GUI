# DISPLACE Documentation

This directory contains the Quarto-based documentation for the DISPLACE project. The documentation is structured as a Quarto book that can be rendered to HTML, PDF, and other formats.

## 📚 What is Quarto?

[Quarto](https://quarto.org/) is an open-source scientific and technical publishing system. It allows us to create documentation that combines narrative text, code, and outputs in a reproducible way.

## 🚀 Quick Start

### Prerequisites

1. **Install Quarto**: Download and install from [https://quarto.org/docs/get-started/](https://quarto.org/docs/get-started/)
2. **Install R** (optional, if using R code chunks): [https://www.r-project.org/](https://www.r-project.org/)

### Preview the Documentation

To preview the documentation locally while editing:

```bash
cd docs
quarto preview
```

This will open a live preview in your browser that automatically updates as you edit files.

### Render the Documentation

To render the complete documentation:

```bash
cd docs
quarto render
```

The rendered HTML files will be placed in the `docs/docs` directory (as specified in `_quarto.yml`).

## 📖 Adding New Content

### Adding a New Chapter

1. **Create a new `.qmd` file** in the `docs/` directory:
   ```bash
   cp _template_chapter.qmd my-new-chapter.qmd
   ```

2. **Edit the file** with your content using Markdown and/or code chunks

3. **Add it to `_quarto.yml`** in the appropriate location:
   ```yaml
   book:
     chapters:
       - index.qmd
       - routines.qmd
       - my-new-chapter.qmd  # Add your chapter here
   ```

4. **Preview your changes**:
   ```bash
   quarto preview
   ```

### Converting Existing Markdown Files

If you have existing `.md` files that you want to include:

1. **Option A - Include as-is**: Quarto can render regular Markdown files. Just add them to `_quarto.yml`:
   ```yaml
   chapters:
     - existing-file.md
   ```

2. **Option B - Convert to `.qmd`**: Rename `.md` to `.qmd` and optionally add YAML frontmatter and code chunks:
   ```bash
   mv existing-file.md existing-file.qmd
   ```

### Organizing with Parts

For better organization, you can group chapters into parts:

```yaml
book:
  chapters:
    - index.qmd
    - part: "Getting Started"
      chapters:
        - quickstart.qmd
        - building.qmd
    - part: "User Guide"
      chapters:
        - routines.qmd
        - running.qmd
```

## 🎨 Customizing the Documentation

### Theme and Appearance

Edit `_quarto.yml` to customize the theme:

```yaml
format:
  html:
    theme: cosmo  # Options: cosmo, flatly, darkly, etc.
    toc: true
    toc_depth: 3
```

### Adding References

1. Add entries to `references.bib` in BibTeX format
2. Cite in your document using `[@citation-key]`
3. References will automatically appear at the end of the document

## 📝 Writing Guidelines

### Headings

Use ATX-style headings with `#`:

```markdown
# Chapter Title (Level 1)
## Section Title (Level 2)
### Subsection Title (Level 3)
```

### Code Chunks

For executable code (R, Python, etc.):

````markdown
```{r}
#| label: my-code-chunk
#| echo: true

# Your R code here
summary(data)
```
````

For non-executable code:

````markdown
```bash
# Your bash commands here
git commit -m "Update docs"
```
````

### Cross-References

Link to other chapters:

```markdown
See @sec-routines for more details.
```

In the target chapter, add a label:

```markdown
# R Routines {#sec-routines}
```

### Images

```markdown
![Caption text](images/my-image.png)
```

Store images in the `docs/images/` directory.

## 🔧 Troubleshooting

### Common Issues

1. **Quarto command not found**
   - Make sure Quarto is installed and in your PATH
   - Restart your terminal after installation

2. **Rendering errors**
   - Check YAML frontmatter syntax
   - Ensure all referenced files exist
   - Review code chunk syntax

3. **Live preview not updating**
   - Save your file to trigger an update
   - Restart the preview with `Ctrl+C` and `quarto preview`

## 📂 Directory Structure

```
docs/
├── _quarto.yml           # Main configuration file
├── index.qmd             # Introduction/home page
├── routines.qmd          # R routines chapter
├── quickstart.qmd        # Quick start guide
├── building.qmd          # Building instructions
├── _template_chapter.qmd # Template for new chapters
├── references.bib        # Bibliography
├── images/               # Image assets
└── docs/                 # Rendered output (git-ignored)
```

## 🤝 Contributing

When contributing to the documentation:

1. **Follow the existing style** and structure
2. **Test your changes** with `quarto preview`
3. **Keep chapters focused** on a single topic
4. **Add cross-references** where appropriate
5. **Include examples** and code snippets when helpful
6. **Check spelling and grammar**

See also [CONTRIBUTING.md](../CONTRIBUTING.md) in the root directory.

## 📚 Resources

- [Quarto Documentation](https://quarto.org/docs/guide/)
- [Quarto Book Guide](https://quarto.org/docs/books/)
- [Markdown Basics](https://quarto.org/docs/authoring/markdown-basics.html)
- [DISPLACE Project Website](https://displace-project.org/)

## ❓ Questions?

If you have questions about the documentation:

1. Check the [Quarto documentation](https://quarto.org/docs/guide/)
2. Open an issue on the GitHub repository
3. Contact the maintainers

---

*Last updated: February 2026*
