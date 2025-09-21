import sys
import json
import azarashi

def load_json_from_file(file_path):
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            return json.load(f)
    except FileNotFoundError:
        print(f"ファイルが見つかりません: {file_path}")
        sys.exit(1)
    except json.JSONDecodeError as e:
        print(f"JSONの読み込みに失敗しました（ファイル）: {e}")
        sys.exit(1)

def load_json_from_stdin():
    try:
        input_data = sys.stdin.read()
        return json.loads(input_data)
    except json.JSONDecodeError as e:
        print(f"JSONの読み込みに失敗しました（標準入力）: {e}")
        sys.exit(1)

def process_messages(messages):
    if not messages:
        print("messages 配列が空です。")
        return

    for idx, message in enumerate(messages, start=1):
        try:
            binary_data = bytes.fromhex(message)
            report = azarashi.decode(binary_data, msg_type='ublox')
            print(f"\n--- Report {idx} ---")
            print(report)
        except Exception as e:
            print(f"\nError decoding message {idx}: {e}")

def main():
    # ファイル名が指定されていればファイルから、なければ標準入力から読み込む
    if len(sys.argv) > 1:
        json_data = load_json_from_file(sys.argv[1])
    else:
        json_data = load_json_from_stdin()

    messages = json_data.get('messages', [])
    process_messages(messages)

if __name__ == "__main__":
    main()
