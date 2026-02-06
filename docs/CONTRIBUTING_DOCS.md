# Contributing to DISPLACE Documentation

Thank you for your interest in improving the DISPLACE documentation! This guide will help you contribute effectively.

## 📋 Table of Contents

- [Getting Started](#getting-started)
- [Documentation Structure](#documentation-structure)
- [Adding New Content](#adding-new-content)
- [Writing Style Guide](#writing-style-guide)
- [Workflow](#workflow)
- [Common Tasks](#common-tasks)
- [Testing Your Changes](#testing-your-changes)

## Getting Started

### Prerequisites

1. **Install Quarto**: Download from [https://quarto.org/docs/get-started/](https://quarto.org/docs/get-started/)
2. **Clone the repository**:
   ```bash
   git clone https://github.com/frabas/DISPLACE_GUI.git
   cd DISPLACE_GUI/docs
   ```
3. **Install R** (optional, for R code chunks): [https://www.r-project.org/](https://www.r-project.org/)

### Quick Test

Verify your setup works:

```bash
cd docs
quarto preview
```

This should open the documentation in your browser.

## Documentation Structure

The documentation is organized as a Quarto book:

```
docs/
├── _quarto.yml              # Main configuration
├── index.qmd                # Introduction/homepage
├── _template_chapter.qmd    # Template for new chapters
├── README.md                # Documentation guide (you're reading it!)
├── CONTRIBUTING_DOCS.md     # This file
├── references.bib           # Bibliography
├── images/                  # Image assets
│
├── Getting Started/
│   ├── quickstart.qmd
│   ├── building.qmd
│   └── running.qmd
│
└── User Guide/
    ├── routines.qmd
    └── output-formats.qmd
```

## Adding New Content

### Option 1: Add a New Chapter

1. **Copy the template**:
   ```bash
   cd docs
   cp _template_chapter.qmd my-new-topic.qmd
   ```

2. **Edit the file**:
   - Update the YAML frontmatter (title, etc.)
   - Write your content using Markdown
   - Add code examples, images, etc. as needed

3. **Add to `_quarto.yml`**:
   ```yaml
   book:
     chapters:
       - index.qmd
       - part: "Your Section"
         chapters:
           - my-new-topic.qmd
   ```

4. **Preview**:
   ```bash
   quarto preview
   ```

### Option 2: Extend Existing Content

1. Open the relevant `.qmd` file
2. Add your content following the existing structure
3. Preview your changes

### Option 3: Convert Existing Markdown

If you have an existing `.md` file:

1. **Rename to `.qmd`**:
   ```bash
   mv existing-doc.md existing-doc.qmd
   ```

2. **Add YAML frontmatter** at the top:
   ```yaml
   ---
   title: "Your Title"
   ---
   ```

3. **Add a section label** after the title:
   ```markdown
   # Your Title {#sec-your-id}
   ```

4. **Add to `_quarto.yml`**

## Writing Style Guide

### General Guidelines

- **Be clear and concise**: Write for users with varying technical backgrounds
- **Use active voice**: "Click the button" vs. "The button should be clicked"
- **Be specific**: Include exact commands, file paths, and examples
- **Keep it current**: Update outdated information
- **Include examples**: Show, don't just tell

### Formatting Conventions

#### Headings

```markdown
# Chapter Title (Level 1 - once per file)
## Major Section (Level 2)
### Subsection (Level 3)
#### Minor point (Level 4)
```

#### Code Blocks

For commands:
````markdown
```bash
cd /path/to/directory
./build.sh
```
````

For configuration files:
````markdown
```yaml
project:
  type: book
  output-dir: ./docs
```
````

For executable code chunks:
````markdown
```{r}
#| label: my-example
#| echo: true
#| eval: false

# R code here
library(displace)
```
````

#### Inline Code

Use backticks for:
- File names: `config.yml`
- Commands: `quarto render`
- Variables: `output_dir`
- Code elements: `function_name()`

#### Callouts

Use Quarto's callout blocks for special content:

```markdown
::: {.callout-note}
This provides additional context.
:::

::: {.callout-tip}
Pro tip: Save time by doing X.
:::

::: {.callout-warning}
Be careful with this configuration!
:::

::: {.callout-important}
Critical information users must know.
:::
```

#### Links

External links:
```markdown
[Quarto Documentation](https://quarto.org)
```

Cross-references to other chapters:
```markdown
See @sec-routines for details.
```

#### Images

```markdown
![Descriptive caption](images/screenshot.png)

Or with more control:

![Descriptive caption](images/screenshot.png){width=80%}
```

Store images in `docs/images/` directory.

#### Lists

Unordered:
```markdown
- First item
- Second item
  - Nested item
```

Ordered:
```markdown
1. First step
2. Second step
3. Third step
```

Task lists:
```markdown
- [x] Completed task
- [ ] Pending task
```

## Workflow

### Making Changes

1. **Create a branch**:
   ```bash
   git checkout -b docs/your-topic
   ```

2. **Make your edits**:
   - Edit `.qmd` files
   - Add images to `docs/images/`
   - Update `_quarto.yml` if adding chapters

3. **Preview locally**:
   ```bash
   cd docs
   quarto preview
   ```

4. **Commit your changes**:
   ```bash
   git add .
   git commit -m "docs: add section on X"
   ```

5. **Push and create PR**:
   ```bash
   git push origin docs/your-topic
   ```

### Commit Message Format

Use conventional commits:

```
docs: add quickstart guide
docs: update building instructions
docs: fix typo in routines chapter
docs: reorganize chapter structure
```

## Common Tasks

### Adding a Screenshot

1. **Capture screenshot** and save to `docs/images/`
2. **Include in document**:
   ```markdown
   ![Screenshot showing the main interface](images/main-interface.png)
   ```

### Adding a Cross-Reference

1. **Label the target section**:
   ```markdown
   ## Important Section {#sec-important}
   ```

2. **Reference it elsewhere**:
   ```markdown
   For more details, see @sec-important.
   ```

### Adding a Citation

1. **Add entry to `references.bib`**:
   ```bibtex
   @article{bastardie2014displace,
     title={DISPLACE: a dynamic, individual-based model...},
     author={Bastardie, Francois and Nielsen, J Rasmus and Miethe, Teunis},
     journal={Canadian Journal of Fisheries and Aquatic Sciences},
     year={2014}
   }
   ```

2. **Cite in text**:
   ```markdown
   The model is described in @bastardie2014displace.
   ```

### Creating a New Part

In `_quarto.yml`:

```yaml
book:
  chapters:
    - index.qmd
    - part: "New Section Name"
      chapters:
        - chapter1.qmd
        - chapter2.qmd
```

## Testing Your Changes

### Before Submitting

1. **Preview locally**: `quarto preview`
2. **Check all links** work
3. **Verify code blocks** are properly formatted
4. **Test cross-references** navigate correctly
5. **Spell-check** your content
6. **Review rendered output** for formatting issues

### Rendering the Full Site

```bash
cd docs
quarto render
```

Check the output in `docs/docs/` directory.

## Questions?

- **Quarto issues**: Check [Quarto docs](https://quarto.org/docs/guide/)
- **Content questions**: Open an issue on GitHub
- **General help**: See main [CONTRIBUTING.md](../CONTRIBUTING.md)

## Resources

- [Quarto Documentation](https://quarto.org/docs/guide/)
- [Quarto Book Guide](https://quarto.org/docs/books/)
- [Markdown Guide](https://www.markdownguide.org/)
- [Quarto Markdown Basics](https://quarto.org/docs/authoring/markdown-basics.html)

---

Thank you for contributing to making DISPLACE documentation better! 🎉
