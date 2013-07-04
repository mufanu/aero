<?php


/**
 * The main classes for the multichoice question type.
 *
 * These inherit or implement code found in quiz_question.classes.inc.
 * Code: LogicMedia
 *
 * Based on:
 * Other question types in the quiz framework.
 *
 *
 *
 * @file
 * Question type, enabling the creation of multiple choice and multiple answer questions.
 */

/**
 * Extension of QuizQuestion.
 */
class QuizfileuploadQuestion extends QuizQuestion {

   /**
   * Implementation of save
   *
   * Stores the question in the database.
   *
   * @param is_new if - if the node is a new node...
   * (non-PHPdoc)
   * @see sites/all/modules/quiz-HEAD/question_types/quiz_question/QuizQuestion#save()
    */
  public function saveNodeProperties($is_new = FALSE) {
    $is_new = $is_new || $this->node->revision == 1;
    if ($is_new) {
      db_insert('quiz_fileupload_node_properties')
        ->fields(array(
          'nid' => $this->node->nid,
          'vid' => $this->node->vid,
          'filetypes' => $this->node->filetypes,
        ))
        ->execute();      
    }
    else {
      db_update('quiz_fileupload_node_properties')
        ->fields(array('filetypes' => $this->node->filetypes))
        ->condition('nid', $this->node->nid)
        ->condition('vid', $this->node->vid)
        ->execute();
    }

  }

  /**
   * Implementation of validate
   *
   * QuizQuestion#validate()
   */
  public function validateNode(array &$form) {
    //no validation required
  }

  /**
   * Implementation of delete
   *
   * @see QuizQuestion#delete()
   */
  public function delete($only_this_version = FALSE) {
    if ($only_this_version) {
      db_delete('quiz_fileupload_node_properties')
        ->condition('question_nid', $this->node->nid)
        ->condition('question_vid', $this->node->vid)
        ->execute();
      db_delete('quiz_fileupload_user_answers')
        ->condition('nid', $this->node->nid)
        ->condition('vid', $this->node->vid)
        ->execute();
    }
    else {
      db_delete('quiz_fileupload_node_properties')
        ->condition('nid', $this->node->nid)
        ->execute();
      db_delete('quiz_fileupload_user_answers')
        ->condition('question_nid', $this->node->nid)
        ->execute();
    }
    parent::delete($only_this_version);
  }

  /**
   * Implementation of getNodeProperties
   *
   * @see QuizQuestion#getNodeProperties()
   */
  public function getNodeProperties() {
    if (isset($this->nodeProperties)) {
      return $this->nodeProperties;
    }
    $props = parent::getNodeProperties();

    // Load the properties
    $res_a = db_query('SELECT filetypes FROM {quiz_fileupload_node_properties} WHERE nid = :nid AND vid = :vid', 
      array(
        ':nid' => $this->node->nid , 
        ':vid' => $this->node->vid))->fetchAssoc();

    if (is_array($res_a)) {
      $props = array_merge($props, $res_a);
    }    
    $this->nodeProperties = $props;
    return $props;
  }

  /**
   * Implementation of getNodeView
   *
   * @see QuizQuestion#getNodeView()
   */
  public function getNodeView() {
    $content = parent::getNodeView();
    $content['filetypes'] = array(
      '#type' => 'markup',
      '#value' => '<pre>' . check_plain($this->node->filetypes) . '</pre>',
    );
    return $content;
  }

  /**
   * Generates the question form.
   *
   * This is called whenever a question is rendered, either
   * to an administrator or to a quiz taker.
   */
  public function getAnsweringForm(array $form_state = NULL, $rid) {
    $form = parent::getAnsweringForm($form_state, $rid);

    $fid = db_query('SELECT qf.fid 
      FROM {quiz_fileupload_user_answers} qf 
      WHERE question_nid = :nid AND question_vid = :vid AND result_id = :result_id',
      array(':nid' => $this->node->nid, ':vid' => $this->node->vid,  ':result_id' => $rid))
      ->fetchField();

    if (is_numeric($fid)) {
      $form['previous_upload'] = array(
        '#title' => t('Previous upload'),
        '#type' => 'item',
        '#markup' => quiz_file_markup($fid),
        '#description' => t('<strong>Upload a new file to replace previous upload.</strong>')
      );
    }
    $form['tries'] = array(
      '#type' => 'file',
      '#title' => t('Upload'),
      '#description' => t('Allowed extensions !ext', array('!ext' => $this->node->filetypes)),
    );
    return $form;
  }

  /**
   * Implementation of getCreationForm
   *
   * @see QuizQuestion#getCreationForm()
   */
  public function getCreationForm(array &$form_state = NULL) {
    $allowed = variable_get('quizfileupload_default_extensions', QUIZFILEUPLOAD_DEFAULT_EXTENSIONS);    
    $form['filetypes'] = array(
      '#type' => 'textfield',
      '#title' => t('Allowed extension'),
      '#description' => t('Enter the allowed file extensions one per line.'),
      '#default_value' => isset($this->node->filetypes) ? $this->node->filetypes : $allowed,
      '#required' => TRUE,
    );
    return $form;
  }

  /**
   * Implementation of getMaximumScore
   *
   * @see QuizQuestion#getMaximumScore()
   */
  public function getMaximumScore() {
    return variable_get('quizfileupload_default_score', 1);
  }

}

/**
 * Extension of QuizQuestionResponse
 */
class QuizfileuploadResponse extends QuizQuestionResponse {
  /**
   * ID of the answer.
   */
  protected $answer_id = 0;

  /**
   * Constructor
   */
  public function __construct($result_id, stdClass $question_node, $tries = NULL) {    
    parent::__construct($result_id, $question_node, $tries);

    $tries = $_FILES;
    $this->answer = $tries;
    $result->is_correct = TRUE;
    $this->question->score_weight = 0;
    $this->evaluated = TRUE;
    $this->result_id = $result_id;

    // Question has been answered allready. We fetch the answer data from the database.
    $r = db_query('SELECT * FROM {quiz_fileupload_user_answers}
    WHERE question_nid = :question_nid AND question_vid = :question_vid AND result_id = :result_id', 
    array(
      ':question_nid' => $question_node->nid, 
      ':question_vid' => $question_node->vid,  
      ':result_id' => $result_id))->fetchAssoc();
    if (is_array($r)) {
      $this->score = $r['score'];
      $this->answer_id = $r['answer_id'];        
    }
    else {
      $this->score = variable_get('quizfileupload_default_score', 0);      
    }
  }

  /**
   * Implementation of isValid
   *
   * @see QuizQuestionResponse#isValid()
   */
  public function isValid() {
    return TRUE; 
  }

  /**
   * Implementation of save
   *
   * @see QuizQuestionResponse#save()
   */
  public function save() {  	
    $file = file_save_upload('tries', array(), 'public://');
    //file_set_status($file, FILE_STATUS_PERMANENT);

    $this->answer_id = db_insert('quiz_fileupload_user_answers')
      ->fields(array(
        'result_id' => $this->rid,
        'question_vid' => $this->question->vid,
        'question_nid' => $this->question->nid,
        'fid' => $file->fid,
        'score' => $this->score(),
      ))
      ->execute();
  }

  /**
   * Implementation of delete
   *
   * @see QuizQuestionResponse#delete()
   */
  public function delete() {
    db_delete('quiz_fileupload_user_answers')
      ->condition('question_nid', $this->question->nid)
      ->condition('question_vid', $this->question->vid)
      ->condition('result_id', $this->rid)
      ->execute();
  }

  /**
   * Implementation of score
   *
   * @return uint
   *
   * @see QuizQuestionResponse#score()
   */
  public function score() {
    return variable_get('quizfileupload_default_score', 0);
  }

  /**
   * Implementation of getResponse
   *
   * @return answer
   *
   * @see QuizQuestionResponse#getResponse()
   */
  public function getResponse() {
    return $this->answer;
  }

  /**
   * Implementation of getReportFormResponse
   *
   * @see getReportFormResponse($showpoints, $showfeedback, $allow_scoring)
   */
  public function getReportFormResponse($showpoints = TRUE, $showfeedback = TRUE, $allow_scoring = FALSE) {
    $result_id = $this->question->answers[0]['result_id'];
    $fid = db_query('SELECT f.fid
      FROM {file_managed} f
      INNER JOIN {quiz_fileupload_user_answers} qf ON (f.fid = qf.fid)
      WHERE result_id = :result_id AND question_nid = :question_nid AND question_vid = :question_vid',
      array(
        ':result_id' => $result_id, 
        ':question_nid' => $this->question->nid, 
        ':question_vid' => $this->question->vid))
        ->fetchField();

    $markup = quiz_file_markup($fid);

    return array(
      '#type' => 'markup',
      '#markup' => $markup);
  }  
}

function quiz_file_markup($fid) {

  if (is_numeric($fid)) {
    // image check
    $file = file_load($fid);
    $errors = file_validate_is_image($file);

    // not image
    if (count($errors)) {
      return l($file->filename , file_create_url($file->uri));
    }
    // image, we use thumbnail
    else {
      $variables['item'] = array(
        'uri' => $file->uri,
        'alt' => '',
        'title' => $file->filename
      );
      $variables['path'] = array(
        'path' => file_create_url($file->uri),
        'options' => array(
          'html' => TRUE
        )
      );
      $variables['image_style'] = 'thumbnail';
      return theme('image_formatter', $variables);
    }
  }
  // no file
  else {
    return t('n/a');
  }
}
