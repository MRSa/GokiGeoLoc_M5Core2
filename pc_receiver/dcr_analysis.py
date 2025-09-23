import argparse
import json
import sys
import serial # pip install pyserial が必要
# import azarashi # azarashiのインポートは使用する直前に行う

def log_error(message):
    """標準エラー出力にエラーメッセージを出力します。"""
    print(f"ERROR: {message}", file=sys.stderr)

def log_info(message):
    """標準エラー出力に情報メッセージを出力します。"""
    print(f"INFO: {message}", file=sys.stderr)

def load_json_from_file(file_path):
    """
    指定されたファイルパスからJSONデータを読み込みます。
    
    Args:
        file_path (str): 読み込むJSONファイルのパス。
        
    Returns:
        dict: 読み込まれたJSONデータ。
    
    Raises:
        FileNotFoundError: ファイルが見つからない場合。
        json.JSONDecodeError: ファイル内容が不正なJSONの場合。
        Exception: その他のI/Oエラー。
    """
    try:
        log_info(f"ファイルを読み込んでいます: {file_path}")
        with open(file_path, 'r', encoding='utf-8') as f:
            return json.load(f)
    except FileNotFoundError:
        log_error(f"ファイルが見つかりません: {file_path}")
        raise
    except json.JSONDecodeError as e:
        log_error(f"JSONファイルの解析エラー: {e}")
        raise
    except Exception as e:
        log_error(f"ファイル読み込み中に予期せぬエラーが発生しました: {e}")
        raise

def load_json_from_serial(port_name, baudrate=115200, timeout=15):
    """
    指定されたシリアルポートからJSONデータを読み込みます。
    
    Args:
        port_name (str): シリアルポート名。
        baudrate (int): ボーレート。
        timeout (int): タイムアウト秒数。
        
    Returns:
        dict: 読み込まれたJSONデータ。
    
    Raises:
        serial.SerialException: ポートへの接続に失敗した場合。
        UnicodeDecodeError: シリアルデータがUTF-8でデコードできない場合。
        json.JSONDecodeError: 受信したデータが不正なJSONの場合。
        TimeoutError: 指定時間内にデータが受信できなかった場合。
    """
    log_info(f"{port_name} に接続しています（{baudrate}bps）...")
    try:
        with serial.Serial(port_name, baudrate=baudrate, timeout=timeout) as ser:
            log_info("コマンド 'CMD:GETDCRMSG' を送信します")
            ser.write(b'CMD:GETDCRMSG\n')

            log_info(f"{port_name}から応答を受信しています。{timeout}秒お待ちください...")
            
            # データ受信
            buffer = b""
            while True:
                line = ser.readline()
                if not line:
                    # タイムアウト
                    raise TimeoutError(f"指定時間 ({timeout}秒) 内にデータを受信できませんでした。")
                
                buffer += line
                
                # 受信したバッファがJSONの開始と終了マーク ('{' と '}') を含んでいるかチェック
                # このチェックは簡易的なもので、より厳密な実装が必要な場合もあります。
                if buffer.startswith(b'{') and buffer.strip().endswith(b'}'):
                    break

            json_str = buffer.decode('utf-8').strip()
            log_info("データ受信完了。JSONを解析します。")
            return json.loads(json_str)

    except serial.SerialException as e:
        log_error(f"シリアルポート接続エラー: {e}")
        raise
    except UnicodeDecodeError as e:
        log_error(f"受信データのデコードエラー: {e}")
        raise
    except json.JSONDecodeError as e:
        log_error(f"受信したJSONデータの解析エラー: {e}")
        raise
    except TimeoutError as e:
        log_error(f"タイムアウトエラー: {e}")
        raise
    except Exception as e:
        log_error(f"シリアル通信中に予期せぬエラーが発生しました: {e}")
        raise

def load_json_from_stdin():
    """標準入力からJSONデータを読み込みます。"""
    log_info("標準入力からJSONデータを読み込みます")
    try:
        input_data = sys.stdin.read()
        if not input_data.strip():
            log_error("標準入力が空です。")
            return None
        return json.loads(input_data)
    except json.JSONDecodeError as e:
        log_error(f"標準入力からのJSON読み込みに失敗しました: {e}")
        raise

def process_messages(messages):
    """
    メッセージのリストを処理し、azarashiライブラリで解析します。
    
    Args:
        messages (list): 16進数文字列のメッセージリスト。
    """
    try:
        # azarashiはここでのみ使用されるため、関数内でインポートします
        import azarashi
    except ImportError:
        log_error("azarashiライブラリが見つかりません。'pip install azarashi'を実行してください。")
        return

    if not messages:
        log_info("messages 配列が空です。")
        return

    for idx, message in enumerate(messages, start=1):
        try:
            binary_data = bytes.fromhex(message)
            report = azarashi.decode(binary_data, msg_type='ublox')
            print(f"\n--- Report {idx} ---")
            print(report)
        except ValueError as e:
            log_error(f"メッセージ {idx} の16進数変換に失敗しました: {e}")
        except Exception as e:
            log_error(f"メッセージ {idx} の解析に失敗しました: {e}")

def main():
    """メイン関数: スクリプトの実行を制御します。"""
    parser = argparse.ArgumentParser(description="JSONデータをファイル、シリアルポート、または標準入力から読み込んで解析します。")
    parser.add_argument('-f', '--file', help='JSONファイルのパス')
    parser.add_argument('-i', '--input', help='シリアルポート名（例: COM3, /dev/ttyUSB0）')
    args = parser.parse_args()

    json_data = None
    try:
        if args.file:
            json_data = load_json_from_file(args.file)
        elif args.input:
            json_data = load_json_from_serial(args.input)
        else:
            log_info("オプションが指定されていません。標準入力から読み込みます。")
            json_data = load_json_from_stdin()
    except (FileNotFoundError, serial.SerialException, UnicodeDecodeError, json.JSONDecodeError, TimeoutError) as e:
        log_error(f"致命的なエラーによりプログラムを終了します: {e}")
        sys.exit(1)
    except Exception as e:
        log_error(f"予期せぬエラーが発生しました: {e}")
        sys.exit(1)

    if json_data and 'messages' in json_data:
        process_messages(json_data['messages'])
    elif json_data is not None:
        log_error("JSONデータに 'messages' キーが見つかりません。")

    log_info("処理を完了しました。")

if __name__ == "__main__":
    main()
