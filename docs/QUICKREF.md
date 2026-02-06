# Quarto Documentation Quick Reference

A cheat sheet for common tasks when working with DISPLACE Quarto documentation.

## 🚀 Quick Commands

```bash
# Preview with live reload
quarto preview

# Render the entire book
quarto render

# Render a single file
quarto render my-chapter.qmd

# Check Quarto version
quarto --version

# Get help
quarto --help
```

## 📝 Common Patterns

### Start a New Chapter

```bash
# 1. Copy template
cp _template_chapter.qmd my-topic.qmd

# 2. Edit my-topic.qmd with your content

# 3. Add to _quarto.yml:
#    - my-topic.qmd

# 4. Preview
quarto preview
```

### Add Frontmatter

```yaml
---
title: "My Chapter Title"
author: "Your Name"
date: last-modified
---
```

### Create Section with ID

```markdown
## My Section {#sec-my-section}
```

### Cross-Reference

```markdown
See @sec-my-section for details.
```

### Code Blocks

**Non-executable:**
````markdown
```bash
cd directory
ls -la
```
````

**Executable R:**
````markdown
```{r}
#| label: fig-plot
#| fig-cap: "My plot"
#| echo: true

plot(1:10)
```
````

**Executable Python:**
````markdown
```{python}
#| label: py-example
#| echo: true

import pandas as pd
df = pd.read_csv("data.csv")
```
````

### Callouts

```markdown
::: {.callout-note}
Note content here
:::

::: {.callout-tip}
Tip content here
:::

::: {.callout-warning}
Warning content here
:::

::: {.callout-important}
Important content here
:::
```

### Images

```markdown
# Simple
![Caption](images/file.png)

# With size control
![Caption](images/file.png){width=80%}

# From code (R)
knitr::include_graphics("images/file.png")
```

### Tables

**Markdown table:**
```markdown
| Column 1 | Column 2 |
|----------|----------|
| Value 1  | Value 2  |
```

**From code (R):**
````markdown
```{r}
#| label: tbl-example
#| tbl-cap: "Table caption"

knitr::kable(head(mtcars))
```
````

### Lists

```markdown
# Unordered
- Item 1
- Item 2
  - Nested item

# Ordered
1. First
2. Second
3. Third

# Task list
- [x] Done
- [ ] Todo
```

### Citations

```markdown
# In text
According to @bastardie2014displace, ...

# Multiple
[@bastardie2014displace; @other2020paper]

# With text
[see @bastardie2014displace, pp. 10-15]
```

### Links

```markdown
# External
[Link text](https://example.com)

# Internal to section
[Link to section](#sec-my-section)

# Cross-reference (preferred)
See @sec-my-section
```

## 📁 File Organization

```
docs/
├── _quarto.yml           # Book configuration
├── index.qmd             # Home page
├── chapter1.qmd          # Chapters
├── chapter2.qmd
├── _template_chapter.qmd # Template
├── references.bib        # Bibliography
└── images/              # Image assets
    └── figure1.png
```

## 🎨 _quarto.yml Structure

```yaml
project:
  type: book
  output-dir: ./docs

book:
  title: "Book Title"
  author: "Author Name"
  date: last-modified
  chapters:
    - index.qmd
    - part: "Section Name"
      chapters:
        - chapter1.qmd
        - chapter2.qmd

bibliography: references.bib

format:
  html:
    theme: cosmo
    toc: true
    number-sections: true
```

## 🔍 Troubleshooting

**Preview not updating?**
- Save the file
- Refresh browser
- Restart preview: `Ctrl+C` then `quarto preview`

**Rendering errors?**
- Check YAML frontmatter syntax
- Verify all file paths are correct
- Check code chunk syntax
- Look for missing closing brackets/quotes

**Cross-references not working?**
- Ensure section has an ID: `{#sec-id}`
- Use `@sec-id` format
- Check for typos in ID names

**Images not showing?**
- Verify file path is correct
- Use relative paths from the .qmd file location
- Check file exists in images/ directory

## 💡 Best Practices

1. **Preview frequently** - Catch errors early
2. **Use section IDs** - Make cross-referencing easier
3. **Keep chapters focused** - One topic per chapter
4. **Add alt text to images** - Improve accessibility
5. **Test all code blocks** - Ensure examples work
6. **Commit often** - Small, focused commits
7. **Follow naming conventions** - Use lowercase-with-hyphens.qmd

## 🎯 Quick Edits Workflow

```bash
# 1. Start preview
cd docs
quarto preview

# 2. Edit files in your editor
# 3. Save - preview auto-updates
# 4. When done, commit changes

git add .
git commit -m "docs: update chapter X"
git push
```

## 📚 Resources

- [Quarto Guide](https://quarto.org/docs/guide/)
- [Quarto Books](https://quarto.org/docs/books/)
- [Markdown Basics](https://quarto.org/docs/authoring/markdown-basics.html)
- [Cross References](https://quarto.org/docs/authoring/cross-references.html)
- [Code Cells](https://quarto.org/docs/computations/r.html)

---

For detailed information, see:
- [README.md](README.md) - Full documentation guide
- [CONTRIBUTING_DOCS.md](CONTRIBUTING_DOCS.md) - Contribution guidelines
- [_template_chapter.qmd](_template_chapter.qmd) - Chapter template
