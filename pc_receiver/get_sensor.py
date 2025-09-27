import argparse
import json
import sys
import serial # pip install pyserial が必要

def log_error(message):
    """標準エラー出力にエラーメッセージを出力します。"""
    print(f"ERROR: {message}", file=sys.stderr)

def log_info(message):
    """標準エラー出力に情報メッセージを出力します。"""
    print(f"INFO: {message}", file=sys.stderr)

def load_json_from_serial(port_name, baudrate=115200, timeout=25):
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
            log_info("コマンド 'CMD:GETSENSOR' を送信します")
            ser.write(b'CMD:GETSENSOR\n')

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

def show_sensor_data(port_number, json_data):
    """ JSONデータを解析してデータを表示する """
    print(f"\n---  {port_number}  ---")
    print(f"  ヒープメモリ  {json_data['heap']}")
    print(f"  バッテリ残量  {json_data['battery']} %")
    print(f"  温度          {json_data['temperature']} ℃  (IMU:{json_data['imu_temp']}) ")
    print(f"  圧力          {json_data['pressure']} Pa")
    print(f"  GPS           {json_data['is_gps']} (緯度: {json_data['lat']}  経度: {json_data['lng']})")
    print(f"  Altitude      {json_data['alt']} m (IMU: {json_data['altitude']})")
    print(f"  Acceleration  (x:{json_data['acc_x']} y:{json_data['acc_y']} z:{json_data['acc_z']})")
    print(f"  Gyro          (x:{json_data['gyro_x']} y:{json_data['gyro_y']} z:{json_data['gyro_z']})")
    print(f"  地磁気        (x:{json_data['mag_x']} y:{json_data['mag_y']} z:{json_data['mag_z']})")
    print("\n\n")


def main():
    """メイン関数: スクリプトの実行を制御します。"""
    parser = argparse.ArgumentParser(description="JSONデータをシリアルポートから読み込んで解析します。")
    parser.add_argument('-i', '--input', help='シリアルポート名（例: COM3, /dev/ttyUSB0）')
    args = parser.parse_args()

    json_data = None
    try:
        if args.input:
            json_data = load_json_from_serial(args.input)
        else:
            log_info("JSONデータをシリアルポートから読み込んで解析します。")
            log_info("オプションが指定されていません。 -i オプションで、読み込むシリアルポートを指定してください。")
            sys.exit(1)
    except (FileNotFoundError, serial.SerialException, UnicodeDecodeError, json.JSONDecodeError, TimeoutError) as e:
        log_error(f"致命的なエラーによりプログラムを終了します: {e}")
        sys.exit(1)
    except Exception as e:
        log_error(f"予期せぬエラーが発生しました: {e}")
        sys.exit(1)

    # ----- データをダンプ
    show_sensor_data(args.input, json_data)
    log_info("処理を完了しました。")

if __name__ == "__main__":
    main()
