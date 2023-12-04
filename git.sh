#!/bin/bash

# 対象ブランチを設定
target_branch="main"

# 現在のブランチを取得
current_branch=$(git rev-parse --abbrev-ref HEAD)

# 現在のブランチが対象ブランチでない場合、スクリプトを終了
if [ "$current_branch" != "$target_branch" ]; then
    echo "Error: This script must be run on the '$target_branch' branch."
    exit 1
fi

# 変更があるかどうかを確認
if ! git diff --quiet; then
    echo "Changes detected. Proceeding with commit..."
else
    echo "No changes detected. Exiting..."
    exit 0
fi

# 'git pull' を実行し、エラーがあればスクリプトを終了
if ! git pull; then
    echo "Error: 'git pull' failed."
    exit 1
fi

# 'git add .' を実行
git add .

# コミットメッセージを生成（日付と時刻を含む）
commit_message=$(date +"%Y/%m/%d %H:%M")

# ユーザーに追加の説明を求める
read -p "Enter additional description for the commit (optional): " extra_description

# 追加の説明がある場合、それをコミットメッセージに追加
if [ ! -z "$extra_description" ]; then
    commit_message="$commit_message - $extra_description"
fi

# コミットを実行し、エラーがあればスクリプトを終了
if ! git commit -m "$commit_message"; then
    echo "Error: 'git commit' failed."
    exit 1
fi

# 'git push' を実行し、エラーがあればスクリプトを終了
if ! git push; then
    echo "Error: 'git push' failed."
    exit 1
fi

echo "Commit and push successful."
