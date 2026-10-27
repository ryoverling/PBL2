#pragma once

#include <QString>

namespace Theme {
    inline QString appStyleSheet() {
        return R"(
            * {
                font-family: "Segoe UI", "Arial";
                font-size: 14px;
                color: #202124;
            }

            QMainWindow, QWidget {
                background: #f6f7fb;
            }

            QLabel#pageTitle {
                font-size: 28px;
                font-weight: 700;
                color: #17181c;
            }

            QLabel#sectionTitle {
                font-size: 17px;
                font-weight: 650;
                color: #26272c;
            }

            QLabel#mutedLabel {
                color: #73757c;
            }

            QFrame#card {
                background: white;
                border: 1px solid #e7e8ed;
                border-radius: 14px;
            }

            QFrame#sidebar {
                background: #16171b;
                border: none;
            }

            QLabel#brand {
                color: white;
                font-size: 20px;
                font-weight: 800;
                padding: 6px;
            }

            QLabel#sidebarUser {
                color: #aeb1ba;
                font-size: 12px;
            }

            QPushButton {
                min-height: 38px;
                border: 1px solid #dfe1e7;
                border-radius: 9px;
                padding: 0 14px;
                background: white;
                font-weight: 600;
            }

            QPushButton:hover {
                background: #f0f1f5;
            }

            QPushButton#primaryButton {
                color: white;
                background: #6c4ed9;
                border: 1px solid #6c4ed9;
            }

            QPushButton#primaryButton:hover {
                background: #5d41c8;
            }

            QPushButton#dangerButton {
                color: #b42318;
                background: #fff7f6;
                border-color: #f2c8c3;
            }

            QPushButton#sidebarButton {
                color: #d9dbe2;
                background: transparent;
                border: none;
                text-align: left;
                padding-left: 18px;
                min-height: 44px;
                border-radius: 9px;
                font-weight: 600;
            }

            QPushButton#sidebarButton:hover {
                background: #24262c;
                color: white;
            }

            QPushButton#sidebarButton:checked {
                background: #6c4ed9;
                color: white;
            }

            QLineEdit, QComboBox, QDateEdit, QDoubleSpinBox, QSpinBox, QTextEdit {
                min-height: 38px;
                background: white;
                border: 1px solid #d9dbe2;
                border-radius: 8px;
                padding: 0 10px;
            }

            QTextEdit {
                padding: 8px 10px;
            }

            QLineEdit:focus, QComboBox:focus, QDateEdit:focus,
            QDoubleSpinBox:focus, QSpinBox:focus, QTextEdit:focus {
                border: 1px solid #6c4ed9;
            }

            QTableWidget {
                background: white;
                border: 1px solid #e3e4e9;
                border-radius: 10px;
                gridline-color: #eceef2;
                selection-background-color: #eeeafd;
                selection-color: #202124;
            }

            QHeaderView::section {
                background: #f3f4f7;
                border: none;
                border-bottom: 1px solid #e3e4e9;
                padding: 9px;
                font-weight: 700;
            }

            QProgressBar {
                border: none;
                border-radius: 7px;
                background: #eceef3;
                text-align: center;
                height: 14px;
            }

            QProgressBar::chunk {
                background: #6c4ed9;
                border-radius: 7px;
            }

            QDialog {
                background: #f8f9fb;
            }
        )";
    }
}
