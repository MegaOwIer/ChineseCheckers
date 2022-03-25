## Task 0: 熟悉 Git 和 Markdown

在本次任务中，你需要在本项目中创建你的小组的文件夹，并添加文件介绍你的小组的简要情况。

### DeadLine

你需要在 2022 年 3 月 31 日 23:59 前完成这一任务并提交 Pull Request。

### 操作步骤

首先如项目根目录的 [REAME.md](https://github.com/MegaOwIer/ChineseCheckers/blob/master/README.md) 所述将本仓库 Fork 到你自己的账号并 clone 到本地。

进入 `docs/submission` 目录，根据你的小组编号创建文件夹并进入

```shell
mkdir Team0
cd Team0
```

然后创建一个新的 Markdown 文件，并在其中说明你的小组成员并标注学号。当然你也可以在这个文件中写一些其他的内容。对 Markdown 不熟悉的同学可以参考 `https://www.runoob.com/markdown/md-tutorial.html` 或自行查找其他资料。

完成上述步骤后，你需要修改项目根目录下的 `mkdocs.yml` 文件中的 `nav` 段，在其中的 `Teams` 列表下添加对应的条目以在网页端显示你的小组主页

```yaml

# ...

nav:
  - Home: index.md
  # ...
  - Teams:
    - Team 0: <path>

# ...

```

其中 `<path>` 是你所创建的文件**相对于 `docs/` 目录的相对路径**。

你可以参考 `docs/submission/Team 0` 以及 `mkdocs.yml` 中 `nav` 段中已有的关于 `Team 0` 的代码段来明确你需要做的事情。

**请注意，在修改 `mkdocs.yml` 时请保持缩进和已有一致（即使用双空格缩进），如果你的编辑器的格式化插件会对此进行修改请提前将其关闭。**

你的更改只有在合并入主仓库并在助教进行统一构建后才能显示在网页端。请同学不要以网页端状态是否改变判断任务是否完成。
