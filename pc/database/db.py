import sqlite3
from datetime import datetime

DB_NAME = "database.db"


def get_connection():
    return sqlite3.connect(DB_NAME)


def init_db():
    conn = get_connection()
    cursor = conn.cursor()

    # Devices table
    cursor.execute("""
    CREATE TABLE IF NOT EXISTS devices (
        node_id INTEGER PRIMARY KEY,
        mac_address TEXT NOT NULL,
        name TEXT NOT NULL
    )
    """)

    # Temperature logs table
    cursor.execute("""
    CREATE TABLE IF NOT EXISTS temperature_logs (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        node_id INTEGER,
        temperature REAL,
        timestamp TEXT,
        FOREIGN KEY (node_id) REFERENCES devices(node_id)
    )
    """)

    conn.commit()
    conn.close()


# ---------------------------
# Device Functions
# ---------------------------

def add_node(node_id, mac, name):
    conn = get_connection()
    cursor = conn.cursor()

    cursor.execute("""
    INSERT OR REPLACE INTO devices (node_id, mac_address, name)
    VALUES (?, ?, ?)
    """, (node_id, mac, name))

    conn.commit()
    conn.close()


def list_all_devices():
    conn = get_connection()
    cursor = conn.cursor()

    cursor.execute("SELECT * FROM devices")
    rows = cursor.fetchall()

    conn.close()
    return rows


def get_mac_address(node_id):
    conn = get_connection()
    cursor = conn.cursor()

    cursor.execute("SELECT mac_address FROM devices WHERE node_id = ?", (node_id,))
    result = cursor.fetchone()

    conn.close()
    return result[0] if result else None


def get_id(name):
    conn = get_connection()
    cursor = conn.cursor()

    cursor.execute("SELECT node_id FROM devices WHERE name = ?", (name,))
    result = cursor.fetchone()

    conn.close()
    return result[0] if result else None


# ---------------------------
# Temperature Log Functions
# ---------------------------

def add_temp_entry(node_id, temp):
    conn = get_connection()
    cursor = conn.cursor()

    timestamp = datetime.now().isoformat()

    cursor.execute("""
    INSERT INTO temperature_logs (node_id, temperature, timestamp)
    VALUES (?, ?, ?)
    """, (node_id, temp, timestamp))

    conn.commit()
    conn.close()


def view_logs(node_id=None):
    conn = get_connection()
    cursor = conn.cursor()

    if node_id:
        cursor.execute("""
        SELECT * FROM temperature_logs
        WHERE node_id = ?
        ORDER BY timestamp DESC
        """, (node_id,))
    else:
        cursor.execute("""
        SELECT * FROM temperature_logs
        ORDER BY timestamp DESC
        """)

    rows = cursor.fetchall()
    conn.close()
    return rows
