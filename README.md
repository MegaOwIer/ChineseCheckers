## Chinese Checkers

本储存仓库用于中国人民大学 2021 级图灵实验班《程序设计 II 荣誉课程》课程大作业的过程记录留档。

你需要将本项目 Fork 到自己的 Github 项目中，提交自己的更新并以 Pull Request 的形式将自己的中期报告等文件提交到本项目中。

### 仓库结构说明

+ `instructions/`：所有的任务文档将以 Markdown 格式发布于该文件夹内。
+ `submission/`：你的所有提交将在该文件夹内完成。
  + 注意，除非任务中有特殊要求，否则你不应该修改位于该文件夹之外的任何文件。

### 使用教程

**除非你已经熟悉 Github 的使用并且知道下面这段内容在做什么，否则请尽量按照下面的提示进行操作。**

首先，你需要根据 GitHub 的官方教程[生成新 SSH 密钥](https://help.github.com/cn/articles/generating-a-new-ssh-key-and-adding-it-to-the-ssh-agent)与[新增 SSH 密钥到 GitHub 帐户](https://help.github.com/cn/articles/adding-a-new-ssh-key-to-your-github-account)，完成对 SSH 密钥的配置。执行 `ssh git@github.com`，出现如下输出则表明配置成功：

```
$ ssh git@github.com
The authenticity of host 'github.com (13.250.177.223)' can't be established.
RSA key fingerprint is SHA256:nThbg6kXUpJWGl7E1IGOCspRomTxdCARLviKw6E5SY8.
Are you sure you want to continue connecting (yes/no/[fingerprint])? yes
Warning: Permanently added 'github.com,13.250.177.223' (RSA) to the list of known hosts.
Hi Menci! You've successfully authenticated, but GitHub does not provide shell access.
Connection to github.com closed.
```

然后，点击 Fork 将本仓库在你的账号下建立一个分支。完成后打开 `https://github.com/<user-name>/ChineseCheckers` 即可查看自己账户下的仓库。

完成上述步骤后，你需要将该项目拉取到本地（需要提前安装 `git`）。这会在你命令行的当前目录新建一个名为 `ChineseCheckers` 的文件夹，请在 `clone` 前确保命令行打开的路径和你的预期相符。

```bash
git clone git@github.com:<user-name>/ChineseCheckers.git
```

随后即可按照每次任务的要求创建相关文件。完成相关修改后你需要使用以下命令向本地 `git` 提交这些修改：

```bash
# cd "ChineseCheckers/foobar/114514"
git add 2021114514张三
git commit
```

在打开的编辑器中输入提交信息（不可为空）后保存并退出编辑器，即可完成本地提交。使用以下命令将提交推送到远程：

```bash
git push
```

如果出现如下输出，表示推送成功：

```bash
$ git push
Enumerating objects: 4, done.
Counting objects: 100% (4/4), done.
Delta compression using up to 4 threads
Compressing objects: 100% (4/4), done.
Writing objects: 100% (4/4), 2.57 KiB | 97.00 KiB/s, done.
Total 4 (delta 0), reused 0 (delta 0)
To github.com:Menci/TuringAdvancedProgramming19A.git
 + bdd68d8...1efda49 master -> master
```

最后，到 GitHub 上点击创建 Pull Request：

在接下来的页面中点击「compare across forks」，并在左边选择主仓库（`MegaOwIer/ChineseCheckers`），右边选择你的仓库。

输入标题和正文（可为空）后点击最下方的「Create pull request」即可。你的提交将在被审阅之后合并到主仓库。
