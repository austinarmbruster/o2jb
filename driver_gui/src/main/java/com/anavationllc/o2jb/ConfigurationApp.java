/*
 * Copyright 2015 AnaVation, LLC. 
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.anavationllc.o2jb;

import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.MissingResourceException;
import java.util.ResourceBundle;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import com.fasterxml.jackson.core.type.TypeReference;
import com.fasterxml.jackson.databind.ObjectMapper;

import javafx.application.Application;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.PasswordField;
import javafx.scene.control.TextField;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyEvent;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.HBox;
import javafx.scene.text.Text;
import javafx.stage.Stage;
import javafx.stage.StageStyle;

public class ConfigurationApp extends Application {
  private static final double BTN_SIZE = 90;

  private static final String RESULT_PATTERN = "{\"action\":\"%s\", \"dsn\":\"%s\", \"cp\":\"%s\", \"driver\":\"%s\", "
      + "\"url\":\"%s\", \"user\":\"%s\", \"password\":\"%s\"}";

  private static final String EMPTY_STRING = "";

  private static final int MAX_COLS = 3;

  private static final int INPUT_COL_SPAN = MAX_COLS - 1;

  private static final double LABEL_WIDTH = 100;

  private static final double TEXT_BOX_WIDTH = 500;

  private static Log LOG = LogFactory.getLog(ConfigurationApp.class);

  private static String _actionTaken = reportAction("failed initialization");

  private ResourceBundle messages = ResourceBundle.getBundle("configurationApp");

  private EventHandler<KeyEvent> keyEventHandler = null;

  private Label classPathLabel = null;

  private TextField classPath = null;

  private Label driverClassLabel = null;

  private TextField driverClass = null;

  private Label jdbcUrlLabel = null;

  private TextField jdbcUrl = null;

  private Label userLabel = null;

  private TextField user = null;

  private Label passwordLabel = null;

  private PasswordField password = null;

  private Label odbcDsnLabel;

  private TextField odbcDsn = null;

  private HBox btnBox = null;

  private Button okBtn = null;

  private Button cancelBtn = null;

  private Stage primaryStage = null;

  private Scene scene = null;

  private GridPane grid = null;

  private Text sceneTitle;

  private Map<String, String> loadedProps = new HashMap<>();

  public static void main(String[] args) {
    getConfig(args);
  }

  private static String reportAction(final String action) {
    return reportAction(action, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
  }

  private static String reportAction(final String action, final String dsn, final String cp, final String driver,
      final String url, final String user, final String password) {
    return String.format(RESULT_PATTERN, action, dsn, cp, driver, url, user, password);
  }

  private static Map<String, String> parseAction(final String json) {
    LOG.info("parsing:  " + json);
    Map<String, String> rtnValue = new HashMap<>();
    ObjectMapper mapper = new ObjectMapper();

    try {
      rtnValue = mapper.readValue(json,
          new TypeReference<HashMap<String, String>>() {
          });
    } catch (Exception e) {
      e.printStackTrace();
    }

    return rtnValue;
  }

  public static String getConfig(String[] args) {
    launch(args);
    LOG.info("Result:  " + _actionTaken);
    return _actionTaken;
  }

  public void start(final Stage primaryStage) throws Exception {

    Parameters params = getParameters();
    List<String> args = params.getRaw();
    if (!args.isEmpty()) {
      loadedProps = parseAction(args.get(0));
    } else {
      LOG.info("No arguments to parse");
    }

    this.primaryStage = primaryStage;
    configureStage();
    primaryStage.show();
  }

  private void configureStage() {
    _actionTaken = reportAction(msg("action.terminated"));
    primaryStage.setTitle("Configure ODBC Settings");
    primaryStage.initStyle(StageStyle.TRANSPARENT);
    primaryStage.setScene(getScene());
  }

  private String msg(String key) {
    String rtnValue = EMPTY_STRING;
    try {
      rtnValue = messages.getString(key);
    } catch (MissingResourceException e) {
      LOG.error("Failed to find the resource key:  " + key, e);
    }
    return rtnValue;
  }

  private EventHandler<KeyEvent> getKeyEventHandler() {
    if (keyEventHandler == null) {
      keyEventHandler = new EventHandler<KeyEvent>() {
        @Override
        public void handle(KeyEvent keyEvent) {
          if (keyEvent.getCode() == KeyCode.ENTER) {
            if (canSubmit()) {
              keyEvent.consume();
              okBtn.fire();
            }
          } else if (keyEvent.getCode() == KeyCode.ESCAPE) {
            keyEvent.consume();
            cancelBtn.fire();
          } else {
            okBtn.setDisable(!canSubmit());
          }
        }
      };

    }
    return keyEventHandler;
  }

  protected boolean canSubmit() {
    return !getOdbcDsn().getText().isEmpty() && !getJdbcUrl().getText().isEmpty()
        && !getClassPath().getText().isEmpty() && !getDriverClass().getText().isEmpty();
  }

  private Label getOdbcDsnLabel() {
    if (odbcDsnLabel == null) {
      odbcDsnLabel = new Label(msg("form.dsn.label"));
      odbcDsnLabel.setMinWidth(Label.USE_PREF_SIZE);
      odbcDsnLabel.setPrefWidth(LABEL_WIDTH);
    }
    return odbcDsnLabel;
  }

  private TextField getOdbcDsn() {
    if (odbcDsn == null) {
      if (loadedProps != null && loadedProps.containsKey("dsn")) {
        odbcDsn = new TextField(loadedProps.get("dsn"));
      } else {
        odbcDsn = new TextField();
      }
      addTextLimiter(odbcDsn, 32);
      odbcDsn.setPromptText(msg("form.dsn.prompt"));
      odbcDsn.setMinWidth(TextField.USE_PREF_SIZE);
      odbcDsn.setPrefWidth(TEXT_BOX_WIDTH);
      odbcDsn.setOnKeyReleased(getKeyEventHandler());
    }
    return odbcDsn;
  }

  private static void addTextLimiter(final TextField tf, final int maxLength) {
    tf.textProperty().addListener(new ChangeListener<String>() {
      @Override
      public void changed(final ObservableValue<? extends String> ov, final String oldValue, final String newValue) {
        if (tf.getText().length() > maxLength) {
          String s = tf.getText().substring(0, maxLength);
          tf.setText(s);
        }
      }
    });
  }

  private Label getJdbcUrlLabel() {
    if (jdbcUrlLabel == null) {
      jdbcUrlLabel = new Label(msg("form.url.label"));
      jdbcUrlLabel.setMinWidth(Label.USE_PREF_SIZE);
      jdbcUrlLabel.setPrefWidth(LABEL_WIDTH);
    }
    return jdbcUrlLabel;
  }

  private TextField getJdbcUrl() {
    if (jdbcUrl == null) {
      jdbcUrl = new TextField(loadedProps != null && loadedProps.containsKey("url") ? loadedProps.get("url") : "");
      jdbcUrl.setPromptText(msg("form.url.prompt"));
      jdbcUrl.setMinWidth(TextField.USE_PREF_SIZE);
      jdbcUrl.setPrefWidth(TEXT_BOX_WIDTH);
      jdbcUrl.setOnKeyReleased(getKeyEventHandler());
    }
    return jdbcUrl;
  }

  private Label getUserLabel() {
    if (userLabel == null) {
      userLabel = new Label(msg("form.user.label"));
      userLabel.setMinWidth(Label.USE_PREF_SIZE);
      userLabel.setPrefWidth(LABEL_WIDTH);
    }
    return userLabel;
  }

  private TextField getUser() {
    if (user == null) {
      user = new TextField(loadedProps != null && loadedProps.containsKey("user") ? loadedProps.get("user") : "");
      user.setPromptText(msg("form.user.prompt"));
      user.setMinWidth(TextField.USE_PREF_SIZE);
      user.setPrefWidth(TEXT_BOX_WIDTH);
      user.setOnKeyReleased(getKeyEventHandler());
    }
    return user;
  }

  private Label getPasswordLabel() {
    if (passwordLabel == null) {
      passwordLabel = new Label(msg("form.password.label"));
      passwordLabel.setMinWidth(Label.USE_PREF_SIZE);
      passwordLabel.setPrefWidth(LABEL_WIDTH);
    }
    return passwordLabel;
  }

  private PasswordField getPassword() {
    if (password == null) {
      password = new PasswordField();
      password.setText(loadedProps != null && loadedProps.containsKey("password") ? loadedProps.get("password") : "");
      password.setPromptText(msg("form.password.prompt"));
      password.setMinWidth(TextField.USE_PREF_SIZE);
      password.setPrefWidth(TEXT_BOX_WIDTH);
      password.setOnKeyReleased(getKeyEventHandler());
    }
    return password;
  }

  private Label getDriverClassLabel() {
    if (driverClassLabel == null) {
      driverClassLabel = new Label(msg("form.driver.label"));
      driverClassLabel.setMinWidth(Label.USE_PREF_SIZE);
      driverClassLabel.setPrefWidth(LABEL_WIDTH);
    }
    return driverClassLabel;
  }

  private TextField getDriverClass() {
    if (driverClass == null) {
      driverClass = new TextField(loadedProps != null && loadedProps.containsKey("driver") ? loadedProps.get("driver") : "");
      driverClass.setPromptText(msg("form.driver.prompt"));
      driverClass.setMinWidth(TextField.USE_PREF_SIZE);
      driverClass.setPrefWidth(TEXT_BOX_WIDTH);
      driverClass.setOnKeyReleased(getKeyEventHandler());
    }
    return driverClass;
  }

  private Label getClassPathLabel() {
    if (classPathLabel == null) {
      classPathLabel = new Label(msg("form.cp.label"));
      classPathLabel.setMinWidth(Label.USE_PREF_SIZE);
      classPathLabel.setPrefWidth(LABEL_WIDTH);
    }
    return classPathLabel;
  }

  private TextField getClassPath() {
    if (classPath == null) {
      classPath = new TextField(loadedProps != null && loadedProps.containsKey("cp") ? loadedProps.get("cp") : "");
      classPath.setPromptText(msg("form.cp.prompt"));
      classPath.setMinWidth(TextField.USE_PREF_SIZE);
      classPath.setPrefWidth(TEXT_BOX_WIDTH);
      classPath.setOnKeyReleased(getKeyEventHandler());
    }
    return classPath;
  }

  private HBox getBtnBox() {
    if (btnBox == null) {
      btnBox = new HBox(10);
      btnBox.setAlignment(Pos.BOTTOM_RIGHT);
      btnBox.getChildren().add(getOkBtn());
      btnBox.getChildren().add(getCancelBtn());
    }
    return btnBox;
  }

  private Button getOkBtn() {
    if (okBtn == null) {
      okBtn = new Button(msg("form.btn.ok"));
      okBtn.setDisable(true);
      okBtn.setMinWidth(Button.USE_PREF_SIZE);
      okBtn.setPrefWidth(90);

      okBtn.setOnAction(new EventHandler<ActionEvent>() {
        @Override
        public void handle(ActionEvent arg0) {
          _actionTaken = reportAction(msg("action.ok"), getOdbcDsn().getText(), getClassPath().getText(),
              getDriverClass().getText(), getJdbcUrl().getText(), getUser().getText(), getPassword().getText());
          close();
        }
      });
    }
    return okBtn;
  }

  private Button getCancelBtn() {
    if (cancelBtn == null) {
      cancelBtn = new Button(msg("form.btn.cancel"));
      cancelBtn.setMinWidth(Button.USE_PREF_SIZE);
      cancelBtn.setPrefWidth(BTN_SIZE);
      cancelBtn.setOnAction(new EventHandler<ActionEvent>() {
        @Override
        public void handle(ActionEvent arg0) {
          _actionTaken = reportAction(msg("action.cancelled"));
          primaryStage.close();
        }
      });
    }
    return cancelBtn;
  }

  protected void close() {
    if (primaryStage != null) {
      primaryStage.close();
    }
  }

  private Scene getScene() {
    if (scene == null) {
      scene = new Scene(getGrid(), 700, 400);
      scene.getStylesheets().add(ClassLoader.getSystemClassLoader().getResource(msg("css.file")).toExternalForm());
    }
    return scene;
  }

  private GridPane getGrid() {
    if (grid == null) {
      grid = new GridPane();
      grid.setAlignment(Pos.CENTER);
      grid.setHgap(10);
      grid.setVgap(10);
      grid.setPadding(new Insets(25, 25, 25, 25));

      int row = 0;
      int col = 0;

      grid.add(getSceneTitle(), col, row, MAX_COLS, 1);
      grid.add(getOdbcDsnLabel(), col = 0, ++row);
      grid.add(getOdbcDsn(), ++col, row, INPUT_COL_SPAN, 1);
      grid.add(getClassPathLabel(), col = 0, ++row);
      grid.add(getClassPath(), ++col, row, INPUT_COL_SPAN, 1);
      grid.add(getDriverClassLabel(), col = 0, ++row);
      grid.add(getDriverClass(), ++col, row, INPUT_COL_SPAN, 1);
      grid.add(getJdbcUrlLabel(), col = 0, ++row);
      grid.add(getJdbcUrl(), ++col, row, INPUT_COL_SPAN, 1);
      grid.add(getUserLabel(), col = 0, ++row);
      grid.add(getUser(), ++col, row, INPUT_COL_SPAN, 1);
      grid.add(getPasswordLabel(), col = 0, ++row);
      grid.add(getPassword(), ++col, row, INPUT_COL_SPAN, 1);
      grid.add(getBtnBox(), col = 1, ++row, INPUT_COL_SPAN, 1);
    }
    return grid;
  }

  private Text getSceneTitle() {
    if (sceneTitle == null) {
      sceneTitle = new Text(msg("form.header"));
      sceneTitle.setId("scene-title-text");
    }
    return sceneTitle;
  }

}